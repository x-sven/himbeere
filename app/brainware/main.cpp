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
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>
#include <boost/timer/timer.hpp> //http://www.boost.org/doc/libs/1_52_0/libs/timer/doc/cpu_timers.html

using namespace std;
namespace po = boost::program_options;
namespace pt = boost::posix_time;

#include "I2Cdev.h"
#include "MPU6050/MPU6050.h"
#include "HMC5883L/HMC5883L.h"
#include "MS561101/MS561101BA.h"
#include "Logging.h"
#include "Drotek10dof.h"
#include "HardwareSerial.h"
#include "AP_GPS.h"
#include "Delay.h"
#include "millis.h"
#include "st_Euler.h"
#include "SensorFusion.h"

#include "ServoInterface/ServoInterface.h"
#include "ControlSystem/ControlSystem.h"
#include "crc16.h"

#include "cDataLink.h"

#define DEBUGMSG(s) cout<<(s)

HardwareSerial gps_usb_serial("/dev/ttyUSB0");
HardwareSerial fcci_serial("/dev/ttyAMA0");

ServoInterface fcci;

AP_GPS_MTK16 gps(&gps_usb_serial);
long gps_timing_buffer = 0;
uint8_t gps_timing_counter = 0;

// I2Cdev::i2c_device_str = "/dev/i2c-1"; //OR
Drotek10dof imu10dof("/dev/i2c-1");

SensorFusion ECF;
ControlSystem CtrlLoop;
uint8_t command_buffer[12];

Logging gpslog;
Logging imulog;
Logging ecflog;

struct st_options
{
    string gcs_udp_addr;
    int logging_rate;
};
st_options options;

cDataLink GCSlink;

bool execute = true;

pt::ptime logging_timer= boost::posix_time::microsec_clock::local_time();

void imu_log(void);
void ecf_imu_update(void);


//***************************************
void trap(int signal)
{
    cout << "\n SIGINT received!" << endl;
    execute = false;

    imu10dof.end();
    imulog.end();

    gps_usb_serial.end();
    gpslog.end();

    ecflog.end();
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
    imulog.begin("logfile_drotek10.log");

    imulog.write(std::ostringstream().flush()
                 << "time_s" << "\t"
                 << "time_us" << "\t"
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

    gps_usb_serial.begin(38400);
    gpslog.begin("logfile_gps-mtk.log");
    gpslog.write(std::ostringstream().flush()
                 << "time_s" << "\t"
                 << "time_us" << "\t"
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
                 << endl
                );

    ecflog.begin("logfile_ecf.log");
    ecflog.write(std::ostringstream().flush()
                 << "time_s" << "\t"
                 << "time_us" << "\t"
                 << "phi_deg" << "\t"
                 << "theta_deg" << "\t"
                 << "psi_deg" << "\t"
                 << "p_deg_s" << "\t"
                 << "q_deg_s" << "\t"
                 << "r_deg_s" << "\t"
                 << "ax_ms2" << "\t"
                 << "ay_ms2" << "\t"
                 << "az_ms2"  << "\t"
                 << endl
                );

    stderr = stdout;
    gps.print_errors = true;
    gps.init();  // GPS Initialization

    fcci.begin(&fcci_serial);

    CtrlLoop.setGainKp(1.0);
    memset(command_buffer,0,12);
    command_buffer[0] = 0xff;
    command_buffer[1] = 0xfe;
}

//***************************************
void imu_log(void)
{

    if(0 < options.logging_rate)
    {
        pt::time_duration logging_interval(boost::posix_time::microseconds(1000000 / options.logging_rate));

        if(pt::microsec_clock::local_time() > logging_timer)
        {
            //cout << "Main: IMU UPDATE!" << endl;
            float acc[3] = {0., 0., 9.81};
            float gyro[3]= {0., 0., 0.};
            float mag[3] = {0., 0., 0.};
            pt::ptime time;

            imu10dof.getScaledIMU(&acc[0], &acc[1], &acc[2], &gyro[0], &gyro[1], &gyro[2], &time);
            imu10dof.getScaledMAG(&mag[0], &mag[1], &mag[2]);

            pt::time_duration difftime = time - pt::from_time_t(0);

            imulog.write(std::ostringstream().flush()
                         << difftime.total_seconds()       << "\t"
                         << difftime.fractional_seconds()  << "\t"
                         << acc[0] << "\t"
                         << acc[1] << "\t"
                         << acc[2] << "\t"
                         << gyro[0] << "\t"
                         << gyro[1] << "\t"
                         << gyro[2] << "\t"
                         << mag[0] << "\t"
                         << mag[1] << "\t"
                         << mag[2] << "\t"
                         << (float)imu10dof.pressure/100. << "\t"
                         << (float)imu10dof.temp/100. << "\t"
                         << (float)imu10dof.getTimingAverage() << "\t"
                         << endl
                        );
            logging_timer += logging_interval;
        } // logging with specified rate
    } // logging disabled
}

//***************************************
void gps_log(void)
{
    gps.update();

    if(0 < options.logging_rate)
    {
        if (gps.new_data)
        {
            pt::time_duration difftime = pt::microsec_clock::local_time() - pt::from_time_t(0);

            gpslog.write(std::ostringstream().flush()
                         << difftime.total_seconds()       << "\t"
                         << difftime.fractional_seconds()  << "\t"
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
    } // if logging not disabled
}

//***************************************
void set_commands(uint16_t ped_us, uint16_t col_us, uint16_t lon_us, uint16_t lat_us)
{
    unsigned int ppm_min=900;
    unsigned int ppm_max=2100;
    uint8_t command_state = 0;

    if(lat_us<ppm_min || lat_us >ppm_max)
        command_state=1;
    if(lon_us<ppm_min || lon_us >ppm_max)
        command_state=1;
    if(ped_us<ppm_min || ped_us >ppm_max)
        command_state=1;
    if(col_us<ppm_min || col_us >ppm_max)
        command_state=1;

    if(0 == command_state)
    {
        command_buffer[2] = (ped_us>>8) & 0xff;
        command_buffer[3] = ped_us & 0xff  ;

        command_buffer[4] = (col_us>>8) & 0xff;
        command_buffer[5] = col_us & 0xff  ;

        command_buffer[6] = (lon_us>>8) & 0xff;
        command_buffer[7] = lon_us & 0xff  ;

        command_buffer[8] = (lat_us>>8) & 0xff;
        command_buffer[9] = lat_us & 0xff  ;


        unsigned short crc = crcsum(&command_buffer[2], 8, 0xffff);

        command_buffer[10] =  crc & 0xff;
        command_buffer[11] =  (crc>>8) & 0xff;


        command_buffer[10] =   0xcc;
        command_buffer[11] =   0xcc;

        fcci_serial.write((uint8_t*)&command_buffer, 12);
    }
}


//***************************************
void ecf_imu_update(void)
{
    pt::time_duration difftime;
    static pt::ptime last_update = pt::microsec_clock::local_time();

    float acc[3] = {0., 0., 9.81};
    float gyro[3]= {0., 0., 0.};

    const float C_DEG2RAD = 0.01745329252;
    const float C_RAD2DEG = 57.2957795131;

    imu10dof.getScaledIMU(&acc[0], &acc[1], &acc[2], &gyro[0], &gyro[1], &gyro[2]);
    //Orientation!?

    ECF.set_AccelVector_mss(acc[0], acc[1], acc[2]);
    ECF.set_GyroVector_rads(gyro[0]*C_DEG2RAD, gyro[1]*C_DEG2RAD, gyro[2]*C_DEG2RAD);

    difftime    = pt::microsec_clock::local_time() - last_update;
    last_update = pt::microsec_clock::local_time();

    ECF.G_Dt = difftime.total_microseconds()/1.e+6;
    ECF.update();

    difftime = pt::microsec_clock::local_time() - pt::from_time_t(0);

    if(pt::microsec_clock::local_time() > logging_timer && 0 < options.logging_rate)
    {
        ecflog.write(std::ostringstream().flush()
                     << difftime.total_seconds()       << "\t"
                     << difftime.fractional_seconds()  << "\t"
                     << ECF.get_euler_angles_rad().toDeg().roll << "\t"
                     << ECF.get_euler_angles_rad().toDeg().pitch << "\t"
                     << ECF.get_euler_angles_rad().toDeg().yaw << "\t"
                     << ECF.get_CorrectedRate_rads(0)*C_RAD2DEG << "\t" //Orientation!!!
                     << ECF.get_CorrectedRate_rads(1)*C_RAD2DEG << "\t"
                     << ECF.get_CorrectedRate_rads(2)*C_RAD2DEG << "\t"
                     << ECF.get_Acceleration_mss(0) << "\t"
                     << ECF.get_Acceleration_mss(1) << "\t"
                     << ECF.get_Acceleration_mss(2)  << "\t"
                     << endl
                    );
    } // if logging


    //CtrlLoop.getServoCmd(float cmd, float state, float i_offset)

    //ECF.get_euler_angles_rad().toDeg().roll

    if(fcci.update())
    {


        printf("Servo channels: ");
        for(uint8_t ch = 0; ch < fcci.num_channel_max; ch++)
        {
            printf("%d ", fcci.get_channel(ch));
        }
        printf("\n");

        set_commands(fcci.get_channel(0),
                     fcci.get_channel(1),
                     fcci.get_channel(2),
                     1500+100.*ECF.get_euler_angles_rad().toDeg().roll);

        //set_commands(1900,1900,1900,1900); //uint16_t ped_us, uint16_t col_us, uint16_t lon_us, uint16_t lat_us

    }

}

//***************************************
void ecf_mag_update(void)
{
    float mag[3] = {0., 0., 0.};

    imu10dof.getScaledMAG(&mag[0], &mag[1], &mag[2]);
    //Orientation!?
    ECF.set_MagVector_Gauss(mag[0], mag[1], mag[2]);
}

bool parse_options(st_options& opt, int argc, char **argv)
{
    try
    {
        // Declare the supported options.
        po::options_description desc("Allowed options");
        desc.add_options()
        ("help", "produce help message")
        ("gcs-ip", po::value<string>(&opt.gcs_udp_addr), "IP address for sending data to.")
        ("lograte", po::value<int>(&opt.logging_rate), "Max. logging data rate in Hz. (set 0 or less to disable it).")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            cout << desc << endl;
            return false;
        }

    }
    catch (exception& e)
    {
        cout << "Error parsing command line: " << e.what() << endl;
        cout << "Use --help option for usage" << endl;
        return false;
    }
    return true;
}

//***************************************
int main(int argc, char **argv )
//***************************************
{
    options.logging_rate = 1000; // max. default logging rate.
    options.gcs_udp_addr = "10.10.10.1"; // default GCS-IP address.

    if (!parse_options(options, argc, argv))
        return 1;

    if(!options.gcs_udp_addr.empty())
    {
        cout << "Trying to send to: "<< options.gcs_udp_addr << endl;
        GCSlink.connect(options.gcs_udp_addr);
    }
    if(0 < options.logging_rate)
    {
        cout << "Max. logging rate set to: "<< options.logging_rate << " Hz." << endl;
    }
    else
    {
        cout << "Logging disabled! "<< endl;
    }
    setup();

    boost::timer::auto_cpu_timer t(1,"Program has within the %w s run time the CPU used for %t s (%p \%).\n\n");

    // register Ctrl-C signal
    signal(SIGINT, &trap);

    /**< register functions, remember not to block the signal as much as necessary! */
    /* Bind IMU's logging to IMU's update signal*/
    //imu10dof.signal_imudata.connect( boost::bind(&DataLogger::update, &imulog) ); //1.2ms without logging
    imu10dof.signal_imudata.connect( (boost::function<void (void)>)imu_log ); //2.6ms with logging

    /* Bind SensorFusions's update to IMU's update signals*/
    imu10dof.signal_imudata.connect((boost::function<void (void)>)ecf_imu_update );
    imu10dof.signal_magdata.connect((boost::function<void (void)>)ecf_mag_update );

    /* Bind Drotek10dof's calibration function to GCSlink's signal*/
    GCSlink.signal_mag_calibration.connect( boost::bind(&Drotek10dof::run_mag_calibration, &imu10dof) );

    //start threads
    imu10dof.begin(100,50,SCHED_FIFO);
    imu10dof.getIMUConfigString("# ");

    // set properties of main thread
    struct sched_param thread_param;
    thread_param.sched_priority = 20; //sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &thread_param);

    //hmm, just wait here - or something to do?
    while(execute)
    {
        //cout << "Nothing to do here!?" << endl;
        // TODO (Sven#1#): Add a signal to GPS
        gps_log();

        //Status Message
        GCSlink.SendStatusMsg(500, 12600); // load in percent*10, voltage in millivolt

        {
            //IMU Message
            float acc[3] = {0., 0., 9.81};
            float gyro[3]= {0., 0., 0.};
            float mag[3] = {0., 0., 0.};

            imu10dof.getScaledIMU(&acc[0], &acc[1], &acc[2], &gyro[0], &gyro[1], &gyro[2]);
            imu10dof.getScaledMAG(&mag[0], &mag[1], &mag[2]);

            GCSlink.SendImuMsg(acc[0] , acc[1] , acc[2],
                               gyro[0], gyro[1], gyro[2],
                               mag[0] , mag[1] , mag[2],
                               (float)imu10dof.pressure/100., (float)imu10dof.temp/100.,
                               0., //p_alt
                               0xffff); //update flags
        }

        {
            // GPS Message
            GCSlink.SendGpsMsg(gps.fix, gps.latitude, gps.longitude, gps.altitude,
                               gps.hdop, 65535,
                               gps.ground_speed , gps.ground_course,
                               gps.num_sats);
        }
        {
            // Attitude Message
            GCSlink.SendAttMsg(ECF.get_euler_angles_rad().roll,
                               ECF.get_euler_angles_rad().pitch,
                               ECF.get_euler_angles_rad().yaw,
                               ECF.get_CorrectedRate_rads(0),
                               ECF.get_CorrectedRate_rads(1),
                               ECF.get_CorrectedRate_rads(2));
        }
        boost::this_thread::sleep(pt::milliseconds(100)); //arround 10 Hz
    }


    cout << "Bye bye!" << endl;
    return(0);
}

