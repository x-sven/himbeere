// includes from: src
#include "cECF/st_Euler.h"

#include "datalink_module.h"

datalink_module::datalink_module(): cDataLink()
{
    m_imu = NULL;
    m_mag = NULL;
    m_baro = NULL;
    m_gps = NULL;
    m_sf  = NULL;
    datalink_module::start();
}

datalink_module::datalink_module(iIMU* _imu, iMAG* _mag, iBARO* _baro, cGPS* _gps, SensorFusion* _sf)
{
    m_imu = _imu;
    m_mag = _mag;
    m_baro = _baro;
    m_gps = _gps;
    m_sf  = _sf;
    datalink_module::start();
}

void datalink_module::start(void)
{

    // create thread
    thread_running = true;
    the_thread = boost::thread( boost::bind(&datalink_module::loop, this));

    struct sched_param thread_param;
    thread_param.sched_priority = 5;
    pthread_t threadID = (pthread_t) the_thread.native_handle();

    int retcode;
    if ((retcode = pthread_setschedparam(threadID, SCHED_FIFO, &thread_param)) != 0)
    {
        errno = retcode;
        perror("pthread_setschedparam");
    }
}

void datalink_module::loop(void)
{
    boost::posix_time::time_duration interval_10Hz(boost::posix_time::milliseconds(1000 / 10));
    boost::posix_time::ptime timer = boost::posix_time::microsec_clock::local_time() + interval_10Hz;

    while(thread_running)
    {
        send_messages();

        boost::this_thread::sleep(timer - boost::posix_time::microsec_clock::local_time());
        timer += interval_10Hz; // update timer
    }

}

void datalink_module::send_messages(void)
{
    //Status Message
    cDataLink::SendStatusMsg(500, 12600); // load in percent*10, voltage in millivolt
    //IMU Message
    if(NULL != m_imu && NULL != m_baro && NULL != m_mag)
    {
        float acc[3] = {0., 0., 9.81};
        float gyro[3]= {0., 0., 0.};
        float mag[3] = {0., 0., 0.};
        float temp = 0;
        float pressure = 0;

        m_imu->getAccel_mss(&acc[0], &acc[1], &acc[2]);
        m_imu->getRates_rads(&gyro[0], &gyro[1], &gyro[2]);
        m_baro->getPressure_pa(&pressure);
        m_baro->getTemperature_deg(&temp);
        m_mag->getField_Gauss(&mag[0], &mag[1], &mag[2]);

        cDataLink::SendImuMsg(  acc[0] , acc[1] , acc[2],
                                gyro[0], gyro[1], gyro[2],
                                mag[0] , mag[1] , mag[2],
                                pressure, temp,
                                0., //p_alt
                                0xffff); //update flags
    }
#if defined(DEBUG)
    else
    {
        std::cout << "Null pointer in datalink_module::send_messages (m_imu, m_baro or m_mag).\n";
    }
#endif
    // GPS Message
    if(NULL != m_gps)
    {
        cDataLink::SendGpsMsg( m_gps->fix, m_gps->latitude, m_gps->longitude, m_gps->altitude,
                               m_gps->hdop, 65535,
                               m_gps->ground_speed , m_gps->ground_course,
                               m_gps->num_sats);
    }
#if defined(DEBUG)
    else
    {
        std::cout << "Null pointer in datalink_module::send_messages (m_gps).\n";
    }
#endif
    if(NULL != m_sf)
    {
        // Attitude Message
        cDataLink::SendAttMsg( m_sf->get_euler_angles_rad().roll,
                               m_sf->get_euler_angles_rad().pitch,
                               m_sf->get_euler_angles_rad().yaw,
                               m_sf->get_CorrectedRate_rads(0),
                               m_sf->get_CorrectedRate_rads(1),
                               m_sf->get_CorrectedRate_rads(2));
    }
#if defined(DEBUG)
    else
    {
        std::cout << "Null pointer in datalink_module::send_messages (m_sf).\n";
    }
#endif
}

datalink_module::~datalink_module()
{
    //dtor
}
