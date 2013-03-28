#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <signal.h>
#include <time.h>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "millis.h"
#include "Logging.h"

timespec ts_beg, ts_end;

bool event_loop_execute = true;

void event_loop_trap(int signal)
{
    event_loop_execute = false;
}



void event_loop(void)
{
    #define frequency 100 //Hz
    int loopcount = 0;
    Logging timelog;

    timelog.begin("logfile_sleep_tester.log");
    timelog.write(std::ostringstream().flush() << "timer_ms\tsleeptime\tcpu_load" << std::endl);

    boost::posix_time::time_duration interval(boost::posix_time::microseconds(1000000 / frequency));
    boost::posix_time::ptime timer = boost::posix_time::microsec_clock::local_time() + interval;

    boost::posix_time::time_duration sleeptime = timer - boost::posix_time::microsec_clock::local_time();

    boost::this_thread::sleep(sleeptime);

    clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
    while(event_loop_execute && loopcount < 500)
    {
#if defined(__arm__)
        for(volatile unsigned int ii=0; ii<20000/4; ii++) //about 40% CPU for raspberry
#else
        for(volatile unsigned int ii=0; ii<20000; ii++) //about 40% CPU for eeePC
#endif
        {
            {
                volatile int n = 0.1;
                volatile double pretendWeNeedTheResult = (cos(n)*sin(n)*pow(n,2));
                pretendWeNeedTheResult = pretendWeNeedTheResult + pretendWeNeedTheResult;
            }
        }
        timelog.write(std::ostringstream().flush() << millis() << "\t"  << sleeptime.total_microseconds() << "\t" << (1-((float)sleeptime.total_microseconds()/(float)interval.total_microseconds())) << std::endl);

        loopcount++;
        timer = timer + interval;
        sleeptime = timer - boost::posix_time::microsec_clock::local_time();
        boost::this_thread::sleep(sleeptime);
    }
    clock_gettime(CLOCK_REALTIME, &ts_end);
    std::cout << "\n ...leaving thread after "
              << loopcount << " loops." << std::endl;

    timelog.end();
}


void test_case_sleep_loop(void)
{

    boost::thread event_thread(&event_loop);
    signal(SIGINT, &event_loop_trap);

    event_thread.join();

    std::cout << (float)( (ts_end.tv_sec - ts_beg.tv_sec)  + (ts_end.tv_nsec - ts_beg.tv_nsec) / 1e9)
              << " s." << std::endl;;
}


void test_case_boosted_thread_loop(void)
{
    boost::thread event_thread(&event_loop);
    signal(SIGINT, &event_loop_trap);

    uint8_t sched_policy = SCHED_FIFO;

    struct sched_param thread_param;
    thread_param.sched_priority = sched_get_priority_max(sched_policy); //select highest prio for scheduler

    pthread_t threadID = (pthread_t) event_thread.native_handle();
    int retcode;
    if ((retcode = pthread_setschedparam(threadID, sched_policy, &thread_param)) != 0)
    {
        errno = retcode;
        perror("pthread_setschedparam");
    }

    std::cout << "Boosted Thread: ";
    std::cout << "policy=" << ((sched_policy == SCHED_FIFO)  ? "SCHED_FIFO" :
                               (sched_policy == SCHED_RR)    ? "SCHED_RR" :
                               (sched_policy == SCHED_OTHER) ? "SCHED_OTHER" :
                               "???")
              << ", priority="  << thread_param.sched_priority
              << " (min/max: "  << sched_get_priority_min(sched_policy) << "/"
              << sched_get_priority_max(sched_policy) << ")" << std::endl;

    event_thread.join();

    std::cout << (float)( (ts_end.tv_sec - ts_beg.tv_sec)  + (ts_end.tv_nsec - ts_beg.tv_nsec) / 1e9)
              << " s." << std::endl;;

}
