#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "Drotek10dof.h"

#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>

#if defined(DEBUG)
#define DEBUGMSG(s) std::cout<<(s)
#else
#define DEBUGMSG(s)
#endif

using namespace std;


Drotek10dof::Drotek10dof()
{
    imu = new MPU6050(MPU6050_ADDRESS_AD0_HIGH); //setDLPFMode?
    mag = new HMC5883L(0x1E);
    baro = new MS561101BA(0x77, MS561101BA_OSR_512);
    //MS561101BA_OSR_512 ==> 0.042 mbar; resolution -> 0,3m; 1ms conversion time; 1.1ms response time
    //MS561101BA_OSR_256 ==> 0.065 mbar; resolution -> 0,5m
    thread_running = false;
    m_imu_timing_buffer = 0;
    m_timing_average = 0.;
}

void Drotek10dof::begin(uint16_t frequency, uint8_t sched_priority, uint8_t sched_policy)
{
    // initialize device
    DEBUGMSG("Initializing I2C devices...\n");

    imu->initialize();
    imu->setFullScaleGyroRange(MPU6050_GYRO_FS_1000);  //+/-1000deg/s
    imu->setFullScaleAccelRange(MPU6050_ACCEL_FS_16);  //+/-16g

    imu->setI2CBypassEnabled(true);
    imu->setI2CMasterModeEnabled(false);

    // master & bypass
    DEBUGMSG("Testing device configuration...\n");
    if (1 == imu->getI2CMasterModeEnabled())
        DEBUGMSG("MPU6050 master mode enabled\n");
    else
        DEBUGMSG("MPU6050 master mode disabled\n");

    if (1 == imu->getI2CBypassEnabled())
        DEBUGMSG("MPU6050 bypass enabled\n");
    else
        DEBUGMSG("MPU6050 bypass disabled\n");

    mag->initialize();
    //continious mode?
    //sample rate?
    //average?

    baro->initialize();
    // settings?

    // verify connection
    DEBUGMSG("Testing device connections...\n");
    DEBUGMSG(imu->testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
    DEBUGMSG("\n");
    DEBUGMSG(baro->testConnection() ? "MS561101BA connection successful" : "MS561101BA connection failed");
    DEBUGMSG("\n");
    DEBUGMSG(mag->testConnection() ? "HMC5883L connection successful" : "HMC5883L connection failed");
    DEBUGMSG("\n");

    // create thread
    m_frequency = frequency; //100 Hz main loop
    thread_running = true;
    imu_thread = boost::thread( boost::bind(&Drotek10dof::loop, this));

    struct sched_param thread_param;
    thread_param.sched_priority = sched_priority;
    pthread_t threadID = (pthread_t) imu_thread.native_handle();

    int retcode;
    if ((retcode = pthread_setschedparam(threadID, sched_policy, &thread_param)) != 0)
    {
        errno = retcode;
        perror("pthread_setschedparam");
    }
#if defined(DEBUG)
    std::cout << "DROTEK Thread: ";
    std::cout << "policy=" << ((sched_policy == SCHED_FIFO)  ? "SCHED_FIFO" :
                               (sched_policy == SCHED_RR)    ? "SCHED_RR" :
                               (sched_policy == SCHED_OTHER) ? "SCHED_OTHER" :
                               "???")
              << ", priority="  << thread_param.sched_priority
              << " (min/max: "  << sched_get_priority_min(sched_policy) << "/"
              << sched_get_priority_max(sched_policy) << ")" << std::endl;
#endif //DEBUG
}

void Drotek10dof::getIMUConfigString(const char* prefix)
{
    //std::ostream* IMUConfigString = new std::ostream();

    //IMUConfigString << prefix;

    switch(imu->getFullScaleGyroRange())
    {
    case(MPU6050_GYRO_FS_250):
        std::cout << prefix << "GYRO: +/-250 deg/s \n";
        break;
    case(MPU6050_GYRO_FS_500):
        std::cout << prefix << "GYRO: +/-500 deg/s \n";
        break;
    case(MPU6050_GYRO_FS_1000):
        std::cout << prefix << "GYRO: +/-1000 deg/s \n";
        break;
    case(MPU6050_GYRO_FS_2000):
        std::cout << prefix << "GYRO: +/-2000 deg/s \n";
        break;
    }

    switch(imu->getFullScaleAccelRange())
    {
    case(MPU6050_ACCEL_FS_2):
        std::cout << prefix << "ACCEL: +/-2g \n";
        break;
    case(MPU6050_ACCEL_FS_4):
        std::cout << prefix << "ACCEL: +/-4g \n";
        break;
    case(MPU6050_ACCEL_FS_8):
        std::cout << prefix << "ACCEL: +/-8g \n";
        break;
    case(MPU6050_ACCEL_FS_16):
        std::cout << prefix << "ACCEL: +/-16g \n";
        break;
    }

    //return(IMUConfigString);
}

void Drotek10dof::connect_imu_callback(boost::function<void (void)> func)
{
    signal_imudata.connect(func);
}

void Drotek10dof::connect_baromag_callback(boost::function<void (void)> func)
{
    signal_baromagdata.connect(func);
}

float Drotek10dof::getTimingAverage(void)
{
    return(m_timing_average);
}

void Drotek10dof::loop(void)
{
    m_imu_timing_buffer  = 0;
    m_imu_timing_counter = 0;
    m_timing_average = 0.0;

    try
    {
        boost::posix_time::time_duration interval_imu(boost::posix_time::microseconds(1000000 / m_frequency));
        boost::posix_time::time_duration interval_50Hz(boost::posix_time::milliseconds(1000 / 50));
        boost::posix_time::time_duration interval_1Hz(boost::posix_time::seconds(1 ));

        boost::posix_time::ptime timer_imu = boost::posix_time::microsec_clock::local_time()  + interval_imu;
        boost::posix_time::ptime timer_50Hz = boost::posix_time::microsec_clock::local_time() + interval_50Hz;
        boost::posix_time::ptime timer_1Hz = boost::posix_time::microsec_clock::local_time()  + interval_1Hz;

        while(thread_running)
        {
            DEBUGMSG("DROTEK Thread loop: IMU\n");
            //update member here...

            // read raw measurements from device
            imu->getMotion6(&ax, &ay, &az, &gx, &gy, &gz); // 1-1.2ms
            //imu.getAcceleration(&ax, &ay, &az); //1.2ms
            //imu.getRotation(&gx, &gy, &gz); //1.2ms

            if (timer_imu > timer_50Hz) //50Hz
            {
                DEBUGMSG("DROTEK thread loop: 50Hz\n");

                mag->getHeading(&mx, &my, &mz); //0.6ms
                baro->read(&pressure, &temp);   //3.6-3.8ms@OSR=1024, 2.4ms@OSR=512
                signal_baromagdata();

                timer_50Hz = timer_50Hz + interval_50Hz;
            }// 50 Hz
            if (timer_imu > timer_1Hz) //1Hz
            {
                DEBUGMSG("DROTEK thread loop: 1Hz\n");

                // update performance counter
                if(0 < m_imu_timing_counter)
                {
                    m_timing_average = (float)m_imu_timing_buffer/(float)m_imu_timing_counter;
                    m_imu_timing_buffer = 0;
                    m_imu_timing_counter = 0;

                    DEBUGMSG("Drotek thread's 1s-mean working time(ms): ");
                    DEBUGMSG(m_timing_average);
                    DEBUGMSG("\n");
                }
                timer_1Hz = timer_1Hz + interval_1Hz;
            }//1Hz

            signal_imudata();

            timer_imu = timer_imu + interval_imu; // update timer

            //performance counter
            boost::posix_time::time_duration spare_time = (timer_imu - boost::posix_time::microsec_clock::local_time()); //spare time
            boost::posix_time::time_duration working_time = interval_imu-spare_time;
            m_imu_timing_buffer += (long)working_time.total_milliseconds();
            m_imu_timing_counter++;
            #if defined(DEBUG)
            std::cout << "t: " << timer_imu << "\ti: " << interval_imu << std::endl;
            #endif
            boost::this_thread::sleep(timer_imu - boost::posix_time::microsec_clock::local_time());
        }//while(1)
        DEBUGMSG("Leaving drotek's thread\n");
    }
    catch(boost::thread_interrupted const& )
    {
        DEBUGMSG("Drotek10dof: serious problems in drotek receive_loop thread.\n");
    }
}

void Drotek10dof::end(void)
{
    thread_running = false;
}

Drotek10dof::~Drotek10dof()
{
    if(NULL != imu) delete(imu);
    if(NULL != mag) delete(mag);
    if(NULL != baro) delete(baro);
}
