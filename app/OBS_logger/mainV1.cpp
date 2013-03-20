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

#include "HardwareSerial.h"
#include "AP_GPS.h"

#include "Delay.h"
#include "millis.h"
#include "Logging.h"

#define DEBUGMSG(s) cout<<(s)

MPU6050 imu(MPU6050_ADDRESS_AD0_HIGH);
HMC5883L mag(0x1E);
MS561101BA baro(0x77, MS561101BA_OSR_512);
//MS561101BA_OSR_512 ==> 0.042 mbar; resolution -> 0,3m; 1ms conversion time; 1.1ms response time
//MS561101BA_OSR_256 ==> 0.065 mbar; resolution -> 0,5m

int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t mx, my, mz;
int32_t pressure, temp;

HardwareSerial Serial("/dev/ttyAMA0");
AP_GPS_MTK16 gps(&Serial);

Logging imulog;
Logging gpslog;

unsigned int IMUCycleCounter = 0;
unsigned int GPSCycleCounter = 0;

long imu_timing_buffer = 0;
uint8_t imu_timing_counter = 0;

long gps_timing_buffer = 0;
uint8_t gps_timing_counter = 0;

boost::asio::io_service io_service;

//***************************************
void trap(int signal)
{
    io_service.stop();
}

//***************************************
void OutputASCII(void)
{

    // display values
    DEBUGMSG("a/g/m/p/T:\t");
    DEBUGMSG(ax);
    DEBUGMSG("\t");
    DEBUGMSG(ay);
    DEBUGMSG("\t");
    DEBUGMSG(az);
    DEBUGMSG("\t");
    DEBUGMSG(gx);
    DEBUGMSG("\t");
    DEBUGMSG(gy);
    DEBUGMSG("\t");
    DEBUGMSG(gz);
    DEBUGMSG("\t");
    DEBUGMSG(mx);
    DEBUGMSG("\t");
    DEBUGMSG(my);
    DEBUGMSG("\t");
    DEBUGMSG(mz);
    DEBUGMSG("\t");
    DEBUGMSG((float)pressure/100.);
    DEBUGMSG("\t");
    DEBUGMSG((float)temp/100.);
    DEBUGMSG("\t");
    DEBUGMSG("\n");
}


//***************************************
void setup()
{
    // initialize device
    DEBUGMSG("Initializing I2C devices...\n");

    imu.initialize();
    imu.setI2CBypassEnabled(true);
    imu.setI2CMasterModeEnabled(false);

    // master & bypass
    DEBUGMSG("Testing device configuration...\n");
    if (1 == imu.getI2CMasterModeEnabled())
        DEBUGMSG("MPU6050 master mode enabled\n");
    else
        DEBUGMSG("MPU6050 master mode disabled\n");

    if (1 == imu.getI2CBypassEnabled())
        DEBUGMSG("MPU6050 bypass enabled\n");
    else
        DEBUGMSG("MPU6050 bypass disabled\n");

    mag.initialize();
    //continious mode?
    //sampla rate?
    //average?

    baro.initialize();
    // settings?

    // verify connection
    DEBUGMSG("Testing device connections...\n");
    DEBUGMSG(imu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
    DEBUGMSG("\n");
    DEBUGMSG(baro.testConnection() ? "MS561101BA connection successful" : "MS561101BA connection failed");
    DEBUGMSG("\n");
    DEBUGMSG(mag.testConnection() ? "HMC5883L connection successful" : "HMC5883L connection failed");
    DEBUGMSG("\n");


    //setup logging
    imulog.begin("logfile_Drotek10.txt");
    imulog.write(std::ostringstream().flush()
                 << "# "
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
                 << "# "
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
void imu_loop(const boost::system::error_code &, boost::asio::deadline_timer* bio_timer)
{
    float timing_average = 0.0;

    bio_timer->expires_from_now(boost::posix_time::milliseconds(10));
    long timer= millis();

    // read raw measurements from device
    imu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); // 1-1.2ms
    //imu.getAcceleration(&ax, &ay, &az); //1.2ms
    //imu.getRotation(&gx, &gy, &gz); //1.2ms

    if (IMUCycleCounter%2) //50Hz
    {
        mag.getHeading(&mx, &my, &mz); //0.6ms
        baro.read(&pressure, &temp);   //3.6-3.8ms@OSR=1024, 2.4ms@OSR=512
    }// 50 Hz

    if (IMUCycleCounter >= 5) //20Hz
    {
        IMUCycleCounter = 0;

        // Performance counter
        if(0 < imu_timing_counter)
        {
            timing_average = (float)imu_timing_buffer/(float)imu_timing_counter;
            imu_timing_buffer = 0;
            imu_timing_counter = 0;
        }
        imulog.write(std::ostringstream().flush()
                     << timer << "\t"
                     << ax << "\t"
                     << ay << "\t"
                     << az << "\t"
                     << gx << "\t"
                     << gy << "\t"
                     << gz << "\t"
                     << mx << "\t"
                     << my << "\t"
                     << mz << "\t"
                     << pressure << "\t"
                     << temp << "\t"
                     << timing_average << "\t"
                     << endl
                    );

        //OutputASCII();
    }// 20Hz

    //performance analysis
    imu_timing_buffer += ( millis()-timer);
    imu_timing_counter++;

    IMUCycleCounter++; //Fast Rate
    bio_timer->async_wait(boost::bind(imu_loop,boost::asio::placeholders::error, bio_timer));
}

//***************************************
void gps_loop(const boost::system::error_code &, boost::asio::deadline_timer* bio_timer)
{
    float timing_average = 0.0;

    bio_timer->expires_from_now(boost::posix_time::milliseconds(50));
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

    bio_timer->async_wait(boost::bind(gps_loop,boost::asio::placeholders::error, bio_timer));
}

//***************************************
int main()
//***************************************
{
    setup();

    //io_service is global

    boost::asio::deadline_timer timer_imu(io_service, boost::posix_time::milliseconds(10));
    timer_imu.async_wait(boost::bind(imu_loop,boost::asio::placeholders::error, &timer_imu));

    boost::asio::deadline_timer timer_gps(io_service, boost::posix_time::milliseconds(50));
    timer_gps.async_wait(boost::bind(gps_loop,boost::asio::placeholders::error, &timer_gps));

    signal(SIGINT, &trap);
    io_service.run();


    cout << "\n SIGINT received, closing log!" << endl;
    imulog.end();
    Serial.end();
    gpslog.end();
}

