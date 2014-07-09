#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>

#include "cGPS.h"

cGPS::cGPS(Stream* s):
    AP_GPS_MTK16(s)
{
    // GPS Initialization
    print_errors = true;
    init();

    // create thread
    thread_running = true;
    gps_thread = boost::thread( boost::bind(&cGPS::loop, this));

    struct sched_param thread_param;
    thread_param.sched_priority = 5;
    pthread_t threadID = (pthread_t) gps_thread.native_handle();

    int retcode;
    if ((retcode = pthread_setschedparam(threadID, SCHED_FIFO, &thread_param)) != 0)
    {
        errno = retcode;
        perror("pthread_setschedparam");
    }
}

void cGPS::loop(void)
{
    boost::posix_time::time_duration interval_10Hz(boost::posix_time::milliseconds(1000 / 10));
    boost::posix_time::ptime timer = boost::posix_time::microsec_clock::local_time() + interval_10Hz;

    while(thread_running)
    {
        AP_GPS_MTK16::update();

        if (AP_GPS_MTK16::new_data)
        {
            new_data = 0;
            signal_newdata();
        }
        boost::this_thread::sleep(timer - boost::posix_time::microsec_clock::local_time());
        timer += interval_10Hz; // update timer
    }
}

std::string cGPS::getString(void)
{
    std::stringstream ss;

    ss  << latitude << "\t"
        << longitude << "\t"
        << altitude << "\t"
        << ground_speed << "\t"
        << ground_course << "\t"
        << (int)num_sats << "\t"
        << fix << "\t"
        << hdop << "\t"
        << time  << "\t"
        << date << "\t"
        << std::endl;
    return(ss.str());
}

cGPS::~cGPS()
{
    thread_running = false;
}
