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

using namespace std;
namespace po = boost::program_options;
namespace pt = boost::posix_time;

#include "I2Cdev.h"
#include "MPU6050/MPU6050.h"
#include "HMC5883L/HMC5883L.h"
#include "MS561101/MS561101BA.h"
#include "Drotek10dof.h"
#include "HardwareSerial.h"
#include "AP_GPS.h"
#include "Delay.h"
#include "millis.h"
#include "Logging.h"
#include "st_Euler.h"
#include "ECF.h"

#include "cDataLink.h"

#define DEBUGMSG(s) cout<<(s)

HardwareSerial Serial("/dev/ttyAMA0");
AP_GPS_MTK16 gps(&Serial);
long gps_timing_buffer = 0;
uint8_t gps_timing_counter = 0;
Logging gpslog;

Drotek10dof imu10dof;
Logging imulog;

ECFClass ECF;
Logging ecflog;

struct st_options
{
    string gcs_udp_addr;
};

cDataLink GCSlink;

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
    gpslog.begin("logfile_gps-mtk.log");
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
                 << endl
                );

    ecflog.begin("logfile_ecf.log");
    ecflog.write(std::ostringstream().flush()
                 << "timer_ms" << "\t"
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

    //Serial.println("GPS MTK library test");
    gps.init();  // GPS Initialization
}

//***************************************
void imu_log(void)
{

    //cout << "Main: IMU UPDATE!" << endl;
    float acc[3] = {0., 0., 9.81};
    float gyro[3]= {0., 0., 0.};
    float mag[3] = {0., 0., 0.};
    pt::ptime time;

    imu10dof.getScaledIMU(&acc[0], &acc[1], &acc[2], &gyro[0], &gyro[1], &gyro[2], &time);
    imu10dof.getScaledMAG(&mag[0], &mag[1], &mag[2]);

    imulog.write(std::ostringstream().flush()
                         << time    << "\t"
//                         << imu10dof.ax << "\t"
//                         << imu10dof.ay << "\t"
//                         << imu10dof.az << "\t"
//                         << imu10dof.gx << "\t"
//                         << imu10dof.gy << "\t"
//                         << imu10dof.gz << "\t"
//                         << imu10dof.mx << "\t"
//                         << imu10dof.my << "\t"
//                         << imu10dof.mz << "\t"
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
                         << imu10dof.getTimingAverage() << "\t"
                         << endl
                        );
}

//***************************************
void gps_log(void)
{
    pt::ptime time = pt::microsec_clock::local_time();

    gps.update();

    if (gps.new_data)
    {
        gpslog.write(std::ostringstream().flush()
                     << time << "\t"
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
}

//***************************************
void ecf_imu_update(void)
{
    float acc[3] = {0., 0., 9.81};
    float gyro[3]= {0., 0., 0.};

    const float C_DEG2RAD = 0.01745329252;
    const float C_RAD2DEG = 57.2957795131;

    imu10dof.getScaledIMU(&acc[0], &acc[1], &acc[2], &gyro[0], &gyro[1], &gyro[2]);
    //Orientation!?

    ECF.set_AccelVector_mss(-acc[0], -acc[1], -acc[2]);
    ECF.set_GyroVector_rads(gyro[0]*C_DEG2RAD, gyro[1]*C_DEG2RAD, gyro[2]*C_DEG2RAD);

    ECF.G_Dt = 0.01; //set real time here!!!
    ECF.update();

    ecflog.write(std::ostringstream().flush()
                 << pt::microsec_clock::local_time() << "\t"
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
}

//***************************************
void ecf_mag_baro_update(void)
{
    float mag[3] = {0., 0., 0.};

    imu10dof.getScaledMAG(&mag[0], &mag[1], &mag[2]);
    //Orientation!?
    ECF.set_MagVector_Gauss(mag[0], mag[1], mag[2]);
}

bool parse_options(st_options& opt, int argc, char **argv)
{
    try {
		// Declare the supported options.
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")

			("gcs-ip", po::value<string>(&opt.gcs_udp_addr), "IP address for sending data to.")
		;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			cout << desc << endl;
			return false;
		}

	} catch (exception& e) {
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
	st_options options;
    if (!parse_options(options, argc, argv))
		return 1;

    if(!options.gcs_udp_addr.empty())
    {
        cout << "Trying to send to: "<< options.gcs_udp_addr << endl;
        GCSlink.connect(options.gcs_udp_addr);
    }
    setup();

    // register Ctrl-C signal
    signal(SIGINT, &trap);

    //register functions
    imu10dof.connect_imu_callback( (boost::function<void (void)>)imu_log );
    imu10dof.connect_imu_callback( (boost::function<void (void)>)ecf_imu_update );
    imu10dof.connect_baromag_callback( (boost::function<void (void)>)ecf_mag_baro_update );

    //start threads
    imu10dof.begin(100,50,SCHED_FIFO);
    imu10dof.getIMUConfigString("# ");

    //hmm, just wait here - or something to do?
    while(execute)
    {
        //cout << "Nothing to do here!?" << endl;
        gps_log();

        //Status Message
        GCSlink.StatusMsg(500, 12600); // load in percent*10, voltage in millivolt

        { //IMU Message
        float acc[3] = {0., 0., 9.81};
        float gyro[3]= {0., 0., 0.};
        float mag[3] = {0., 0., 0.};

        imu10dof.getScaledIMU(&acc[0], &acc[1], &acc[2], &gyro[0], &gyro[1], &gyro[2]);
        imu10dof.getScaledMAG(&mag[0], &mag[1], &mag[2]);

        GCSlink.ImuMsg(acc[0] , acc[1] , acc[2],
                       gyro[0], gyro[1], gyro[2],
                       mag[0] , mag[1] , mag[2],
                       (float)imu10dof.pressure/100., (float)imu10dof.temp/100.,
                        0., //p_alt
                        0xffff); //update flags
        }

        { // GPS Message
        GCSlink.GpsMsg(gps.fix, gps.latitude, gps.longitude, gps.altitude,
                       gps.hdop, 65535,
                       gps.ground_speed , gps.ground_course,
                       gps.num_sats);
        }
        { // Attitude Message
        GCSlink.AttMsg(ECF.get_euler_angles_rad().roll,
                       ECF.get_euler_angles_rad().pitch,
                       ECF.get_euler_angles_rad().yaw,
                       ECF.get_CorrectedRate_rads(0),
                       ECF.get_CorrectedRate_rads(1),
                       ECF.get_CorrectedRate_rads(2));
        }
        boost::this_thread::sleep(pt::milliseconds(100)); //arround 10 Hz
    }


    cout << "\n Bye bye!" << endl;
    return(0);
}

