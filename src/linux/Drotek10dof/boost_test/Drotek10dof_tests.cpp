#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <cmath>
#include <ostream>
#include <inttypes.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/thread.hpp>

//!< First read this: http://www.boost.org/doc/libs/1_53_0/libs/test/doc/html/utf/user-guide.html

//!< The following define is used to access privat methods
//!< without modifications of the included classes.
#define private public

#include "Drotek10dof.h"

using namespace std;

Drotek10dof imu10dof;
int cb_counter = 0;
boost::posix_time::ptime start_time;
bool thread_running = false;

void imu_cb_counter(void)
{
    boost::posix_time::ptime cb_time = (boost::posix_time::microsec_clock::local_time());
    boost::posix_time::time_duration interval_imu(boost::posix_time::microseconds(1000000 / 200)); //100 Hz
    boost::posix_time::time_duration time_error = start_time - cb_time + interval_imu;

    if(cb_counter > 10) // skip first 10 calls
    {
        BOOST_CHECK_SMALL((double)time_error.total_microseconds() , 1000.);
    }
//    cout<< boost::posix_time::to_simple_string(cb_time)
//        << " == "
//        << boost::posix_time::to_simple_string(start_time+interval_imu)
//        << " ?" << endl;
    cb_counter++;
    start_time = (boost::posix_time::microsec_clock::local_time());
}

void while_function(void)
{
    volatile int ii = 0;
    while(thread_running)
    {
        //waste time;
        ii++;
        boost::this_thread::sleep(boost::posix_time::microsec(10));
        ii--;
    }

}


BOOST_AUTO_TEST_CASE( call_back_timing )
{
    //register function
    imu10dof.signal_imudata.connect( (boost::function<void (void)>)imu_cb_counter );

    start_time = (boost::posix_time::microsec_clock::local_time());

    //start thread
    imu10dof.begin(200,50,SCHED_FIFO);

    //wait
    boost::this_thread::sleep(boost::posix_time::milliseconds(999));

    //check
    BOOST_CHECK_EQUAL(cb_counter, 200); //1 sec -> 200 loop calls!?

    //std::cout << "*****Starting Thread*****" << std::endl;

    thread_running = true;
    boost::thread time_wasting_thread(&while_function);

    //std::cout << "********Waiting********" << std::endl;

    //wait
    boost::this_thread::sleep(boost::posix_time::milliseconds(999));
    thread_running = false;

    //std::cout << "********Terminated********" << std::endl;

    BOOST_CHECK_EQUAL(cb_counter, 400); //1 sec -> 200 loop calls!?

    //terminate
    imu10dof.end();

//    std::cout << "Drotek IMU update takes: "
//              << (float)imu10dof.getTimingAverage()
//              << " ms (avg./1s)."
//              << std::endl;
    BOOST_CHECK_LE((float)imu10dof.getTimingAverage(), 1.5); //ms

    float acc[3] = {0., 0., 9.81};
    float gyro[3]= {0., 0., 0.};
    float mag[3] = {0., 0., 0.};
    boost::posix_time::ptime time;

    imu10dof.getScaledIMU(&acc[0], &acc[1], &acc[2], &gyro[0], &gyro[1], &gyro[2], &time);
    BOOST_CHECK_LE( fabs(acc[0]), 1.);
    BOOST_CHECK_LE( fabs(acc[1]), 1.);
    BOOST_CHECK_CLOSE_FRACTION( fabs(acc[2]), 10.0, 1.);
    BOOST_CHECK_LE( fabs(gyro[0]), 1.);
    BOOST_CHECK_LE( fabs(gyro[1]), 1.);
    BOOST_CHECK_LE( fabs(gyro[2]), 1.);
    BOOST_CHECK_LE( (boost::posix_time::microsec_clock::local_time()-time).total_milliseconds(), 10 );

    imu10dof.getScaledMAG(&mag[0], &mag[1], &mag[2], &time);
    BOOST_CHECK_LE( fabs(mag[0]), 1.);
    BOOST_CHECK_LE( fabs(mag[1]), 1.);
    BOOST_CHECK_LE( fabs(mag[2]), 1.);
    BOOST_CHECK_LE( (boost::posix_time::microsec_clock::local_time()-time).total_milliseconds(), 30 );

    BOOST_CHECK_CLOSE_FRACTION( (float)imu10dof.pressure/100. , 1013., 10);
    BOOST_CHECK_CLOSE_FRACTION( (float)imu10dof.temp/100. , 20., 10);
    BOOST_CHECK_LE( (boost::posix_time::microsec_clock::local_time()-imu10dof.time_baro).total_milliseconds(), 30 );

}

#include <MS561101/MS561101BA.h>
BOOST_AUTO_TEST_CASE( sensor_timing_baro )
{
    int32_t pressure = 0, temp = 0;

    /*MS561101BA_OSR_256----------------------------------------------*/
    {
        MS561101BA *baro = new MS561101BA(0x77, MS561101BA_OSR_256);

        baro->initialize();
        BOOST_CHECK(baro->testConnection());

        start_time = (boost::posix_time::microsec_clock::local_time());
        baro->read(&pressure, &temp);   //3.6-3.8ms@OSR=1024, 2.4ms@OSR=512
        boost::posix_time::time_duration required_time = boost::posix_time::microsec_clock::local_time()-start_time;

//        std::cout << "Baro (MS561101BA_OSR_256) gives: "
//                  << pressure << ", "
//                  << temp << ", in: "
//                  << required_time.total_microseconds()
//                  << " us." << std::endl;
        BOOST_CHECK_LE( required_time.total_microseconds(), 3000 );

        if(NULL != baro) delete(baro);
    }
    /*MS561101BA_OSR_512----------------------------------------------*/
    {
        MS561101BA *baro = new MS561101BA(0x77, MS561101BA_OSR_512);

        baro->initialize();
        BOOST_CHECK(baro->testConnection());

        start_time = (boost::posix_time::microsec_clock::local_time());
        baro->read(&pressure, &temp);   //3.6-3.8ms@OSR=1024, 2.4ms@OSR=512
        boost::posix_time::time_duration required_time = boost::posix_time::microsec_clock::local_time()-start_time;

//       std::cout << "Baro (MS561101BA_OSR_512) gives: "
//                  << pressure << ", "
//                  << temp << ", in: "
//                  << required_time.total_microseconds()
//                  << " us." << std::endl;
        BOOST_CHECK_LE( required_time.total_microseconds(), 4000 );

        if(NULL != baro) delete(baro);
    }
    /*MS561101BA_OSR_4096----------------------------------------------*/
    {
        MS561101BA *baro = new MS561101BA(0x77, MS561101BA_OSR_4096);

        baro->initialize();
        BOOST_CHECK(baro->testConnection());

        start_time = (boost::posix_time::microsec_clock::local_time());
        baro->read(&pressure, &temp);   //3.6-3.8ms@OSR=1024, 2.4ms@OSR=512
        boost::posix_time::time_duration required_time = boost::posix_time::microsec_clock::local_time()-start_time;

//        std::cout << "Baro (MS561101BA_OSR_4096) gives: "
//                  << pressure << ", "
//                  << temp << ", in: "
//                  << required_time.total_microseconds()
//                  << " us." << std::endl;
        BOOST_CHECK_LE( required_time.total_microseconds(), 25000 );

        if(NULL != baro) delete(baro);
    }
    /*MS561101BA_OSR_512 with 4-step reading---------------------------*/
    {
        MS561101BA *baro = new MS561101BA(0x77, MS561101BA_OSR_512);

        baro->initialize();
        BOOST_CHECK(baro->testConnection());

        start_time = (boost::posix_time::microsec_clock::local_time());
        /*1*/
        uint16_t delay = baro->StartPressureConversion();
        boost::this_thread::sleep(boost::posix_time::microseconds(delay));
        /*2*/
        baro->ReadPressure();
        /*3*/
        delay = baro->StartTemperatureConversion();
        boost::this_thread::sleep(boost::posix_time::microseconds(delay));
        /*4*/
        baro->ReadTemperature();
        baro->getValues(&pressure, &temp);

        boost::posix_time::time_duration required_time = boost::posix_time::microsec_clock::local_time()-start_time;

//        std::cout << "4-Step Baro (MS561101BA_OSR_512) gives: "
//                  << pressure << ", "
//                  << temp << ", in: "
//                  << required_time.total_microseconds()
//                  << " us." << std::endl;
        BOOST_CHECK_LE( required_time.total_microseconds(), 4500 );

        if(NULL != baro) delete(baro);
    }
}


#include <HMC5883L/HMC5883L.h>
BOOST_AUTO_TEST_CASE( sensor_timing_mag )
{
    int16_t mx=0, my=0, mz=0;

    {
        HMC5883L *mag = new HMC5883L(0x1E);

        mag->initialize();
        BOOST_CHECK(mag->testConnection());

        if(mag->testConnection())
        {
            //Select number of samples averaged (1 to 8) per
            // measurement output.
            mag->setSampleAveraging(HMC5883L_AVERAGING_8);
            // 1370 LSB/Gauss  => ± 0.88 Ga recommended, range 2048/1370 = ±1,49 Ga
            mag->setGain(HMC5883L_GAIN_1370);
            // Typical Data Output Rate (Hz) => 75Hz
            mag->setDataRate(HMC5883L_RATE_75);
            // During continuous-measurement mode, the device continuously
            // makes measurements, at user selectable rate, and
            // places measured data in data output registers.
            mag->setMode(HMC5883L_MODE_CONTINUOUS);
        }


        start_time = (boost::posix_time::microsec_clock::local_time());
        mag->getMeasurments(&mx, &my, &mz); //0.6ms
        boost::posix_time::time_duration required_time = boost::posix_time::microsec_clock::local_time()-start_time;

//        std::cout << "MAG Sensor Reading (HMC5883L_MODE_CONTINUOUS) takes: "
//                  << required_time.total_microseconds()
//                  << " us." << std::endl;
        BOOST_CHECK_LE( required_time.total_microseconds(), 400 );

        if(NULL != mag) delete(mag);
    }
    /*-------------------------------*/
    {
        HMC5883L *mag = new HMC5883L(0x1E);

        mag->initialize();
        BOOST_CHECK(mag->testConnection());

        if(mag->testConnection())
        {
            //Select number of samples averaged (1 to 8) per
            // measurement output.
            mag->setSampleAveraging(HMC5883L_AVERAGING_8);
            // 1370 LSB/Gauss  => ± 0.88 Ga recommended, range 2048/1370 = ±1,49 Ga
            mag->setGain(HMC5883L_GAIN_1370);
//            // Typical Data Output Rate (Hz) => 75Hz
//            mag->setDataRate(HMC5883L_RATE_75);
            // During continuous-measurement mode, the device continuously
            // makes measurements, at user selectable rate, and
            // places measured data in data output registers.
            mag->setMode(HMC5883L_MODE_SINGLE);
        }


        start_time = (boost::posix_time::microsec_clock::local_time());
        mag->getMeasurments(&mx, &my, &mz); //0.6ms
        boost::posix_time::time_duration required_time = boost::posix_time::microsec_clock::local_time()-start_time;

//        std::cout << "MAG Sensor Reading (HMC5883L_MODE_SINGLE) takes: "
//                  << required_time.total_microseconds()
//                  << " us." << std::endl;
        BOOST_CHECK_LE( required_time.total_microseconds(), 600 );

        if(NULL != mag) delete(mag);
    }
}

#include <MPU6050/MPU6050.h>
BOOST_AUTO_TEST_CASE( sensor_timing_imu )
{
    int16_t ax=0, ay=0, az=0;
    int16_t gx=0, gy=0, gz=0;

    MPU6050 *imu = new MPU6050(MPU6050_ADDRESS_AD0_HIGH); //setDLPFMode?

    imu->initialize();
    BOOST_CHECK(imu->testConnection());

    imu->setFullScaleGyroRange(MPU6050_GYRO_FS_1000);  //+/-1000deg/s
    imu->setFullScaleAccelRange(MPU6050_ACCEL_FS_16);  //+/-16g

    imu->setI2CBypassEnabled(true);
    imu->setI2CMasterModeEnabled(false);

    start_time = (boost::posix_time::microsec_clock::local_time());
    imu->getMotion6(&ax, &ay, &az, &gx, &gy, &gz); // 1-1.2ms
    boost::posix_time::time_duration required_time = boost::posix_time::microsec_clock::local_time()-start_time;

//    std::cout << "IMU Sensor Reading takes: "
//              << required_time.total_microseconds()
//              << " us." << std::endl;
    BOOST_CHECK_LE( required_time.total_microseconds(), 600 );

    if(NULL != imu) delete(imu);
}
