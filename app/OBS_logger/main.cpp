#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <signal.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace std;

#include <I2Cdev.h>
#include <MPU6050/MPU6050.h>
#include <HMC5883L/HMC5883L.h>
#include <MS561101/MS561101BA.h>

#include <Drotek10dof.h>


#include "HardwareSerial.h"
#include "AP_GPS.h"

#include "Delay.h"
#include "millis.h"
#include "Logging.h"

#define DEBUGMSG(s) cout<<(s)

HardwareSerial Serial("/dev/ttyAMA0");
AP_GPS_MTK16 gps(&Serial);
long gps_timing_buffer = 0;
uint8_t gps_timing_counter = 0;
Logging gpslog;

Drotek10dof imu10dof;
Logging imulog;

bool execute = true;

//***************************************
void trap(int signal)
{
    cout << "\n SIGINT received!" << endl;
    execute = false;

    imu10dof.end();
    imulog.end();

    Serial.end();
    gpslog.end();
}

//***************************************
void OutputASCII(void)
{

    // display values
    DEBUGMSG("a/g/m/p/T:\t");
    DEBUGMSG(imu10dof.ax);
    DEBUGMSG("\t");
    DEBUGMSG(imu10dof.ay);
    DEBUGMSG("\t");
    DEBUGMSG(imu10dof.az);
    DEBUGMSG("\t");
    DEBUGMSG(imu10dof.gx);
    DEBUGMSG("\t");
    DEBUGMSG(imu10dof.gy);
    DEBUGMSG("\t");
    DEBUGMSG(imu10dof.gz);
    DEBUGMSG("\t");
    DEBUGMSG(imu10dof.mx);
    DEBUGMSG("\t");
    DEBUGMSG(imu10dof.my);
    DEBUGMSG("\t");
    DEBUGMSG(imu10dof.mz);
    DEBUGMSG("\t");
    DEBUGMSG((float)imu10dof.pressure/100.);
    DEBUGMSG("\t");
    DEBUGMSG((float)imu10dof.temp/100.);
    DEBUGMSG("\t");
    DEBUGMSG("\n");
}


//***************************************
void setup()
{
    //setup logging
    imulog.begin("logfile_Drotek10.txt");

    imu10dof.getIMUConfigString("# ");

    imulog.write(std::ostringstream().flush()
                 << "timer_ms" << "\t"
                 << "ax" << "\t"
                 << "ay" << "\t"
                 << "az" << "\t"
                 << "gx" << "\t"
                 << "gy" << "\t"
                 << "gz" << "\t"
                 << "mx" << "\t"
                 << "my" << "\t"
                 << "mz" << "\t"
                 << "pressure" << "\t"
                 << "temp" << "\t"
                 << "runtime_average_ms" << "\t"
                 << endl
                );

    Serial.begin(38400);
    gpslog.begin("logfile_gps-mtk.txt");
    gpslog.write(std::ostringstream().flush()
                 << "timer_ms" << "\t"
                 << "latitude" << "\t"
                 << "longitude" << "\t"
                 << "altitude" << "\t"
                 << "ground_speed" << "\t"
                 << "ground_course" << "\t"
                 << "num_sats" << "\t"
                 << "fix" << "\t"
                 << "hdop" << "\t"
                 << "time"  << "\t"
                 << "date" << "\t"
                 << "timing_average_ms" << "\t"
                 << endl
                );

    stderr = stdout;
    gps.print_errors = true;

    //Serial.println("GPS MTK library test");
    gps.init();  // GPS Initialization
}


//***************************************
void imu_log(void)
{

    //cout << "Main: IMU UPDATE!" << endl;

    imulog.write(std::ostringstream().flush()
                         << millis()    << "\t"
                         << imu10dof.ax << "\t"
                         << imu10dof.ay << "\t"
                         << imu10dof.az << "\t"
                         << imu10dof.gx << "\t"
                         << imu10dof.gy << "\t"
                         << imu10dof.gz << "\t"
                         << imu10dof.mx << "\t"
                         << imu10dof.my << "\t"
                         << imu10dof.mz << "\t"
                         << imu10dof.pressure << "\t"
                         << imu10dof.temp << "\t"
                         << imu10dof.getTimingAverage() << "\t"
                         << endl
                        );
}

//***************************************
void gps_log(void)
{
    float timing_average = 0.0;
    long timer= millis();

    gps.update();

    if (gps.new_data)
    {
        // Performance counter
        if(0 < gps_timing_counter)
        {
            timing_average = (float)gps_timing_buffer/(float)gps_timing_counter;
            gps_timing_buffer = 0;
            gps_timing_counter = 0;
        }
        gpslog.write(std::ostringstream().flush()
                     << timer << "\t"
                     << gps.latitude << "\t"
                     << gps.longitude << "\t"
                     << gps.altitude << "\t"
                     << gps.ground_speed << "\t"
                     << gps.ground_course << "\t"
                     << (int)gps.num_sats << "\t"
                     << gps.fix << "\t"
                     << gps.hdop << "\t"
                     << gps.time  << "\t"
                     << gps.date << "\t"
                     << timing_average << "\t"
                     << endl
                    );
//        std::cout <<"----GPS----"                    << std::endl;
//        std::cout <<" Lat: "
//                  << (float)gps.latitude/1.e+7       << std::endl;
//        std::cout <<" Lon: "
//                  << (float)gps.longitude/1.e+7      << std::endl;
//        std::cout <<" Alt: "
//                  << (float)gps.altitude / 100.0     << std::endl;
//        std::cout <<" Speed: "
//                  << (float)gps.ground_speed / 100.0 << std::endl;
//        std::cout <<" Course: "
//                  << (float)gps.ground_course / 100.0<< std::endl;
//        std::cout <<" SAT: "
//                  << (int)gps.num_sats               << std::endl;
//        std::cout <<" FIX: "
//                  << (int)gps.fix                    << std::endl;
//        std::cout <<" HDOP: "
//                  << (float)gps.hdop / 100.0         << std::endl;
//        std::cout <<" TIME: "
//                  << gps.time                        << std::endl;
//        std::cout <<" DATE: "
//                  << gps.date                        << std::endl;
        gps.new_data = 0; // We have read the data
        //OutputASCII();
    }// ...for new data

    //performance analysis
    gps_timing_buffer += ( millis()-timer);
    gps_timing_counter++;
}


//***************************************
int main()
//***************************************
{
    setup();

    // register Ctrl-C signal
    signal(SIGINT, &trap);


    //register functions
    imu10dof.connect_imu_callback( (boost::function<void (void)>)imu_log );


    //start threads
    imu10dof.begin(100,10,SCHED_FIFO);

    //hmm, just wait here - or something to do?
    while(execute)
    {
        //cout << "Nothing to do here!?" << endl;

        gps_log();
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }


    cout << "\n Bye bye!" << endl;
    return(0);
}

