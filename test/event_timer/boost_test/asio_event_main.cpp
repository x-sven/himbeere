#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "millis.h"
#include "Logging.h"

int loopcount = 0;
Logging timelog;

void time_based_function(const boost::system::error_code &, boost::asio::deadline_timer* timer) // = IMU LOOP
{
#define frequency 100 //Hz
    timer->expires_from_now(boost::posix_time::milliseconds(1000/frequency));

    boost::posix_time::time_duration interval(boost::posix_time::microseconds(1000000 / frequency));
    boost::posix_time::ptime ptimer = boost::posix_time::microsec_clock::local_time() + interval;

    for(volatile unsigned int ii=0; ii<20000; ii++) //about 40% CPU for eeePC
    {
        {
            volatile int n = 0.1;
            volatile double pretendWeNeedTheResult = (cos(n)*sin(n)*pow(n,2));
            pretendWeNeedTheResult = pretendWeNeedTheResult + pretendWeNeedTheResult;
        }
    }
    loopcount++;
    boost::posix_time::time_duration sleeptime = ptimer - boost::posix_time::microsec_clock::local_time();
    timelog.write(std::ostringstream().flush() << millis() << "\t"  << sleeptime.total_microseconds() << "\t" << (1-((float)sleeptime.total_microseconds()/(float)interval.total_microseconds())) << std::endl);

    if(loopcount < 500)
    {
        timer->async_wait(boost::bind(time_based_function,boost::asio::placeholders::error, timer));
    }

    timelog.end();
}

void self_blocking_function(void) // = GPS LOOP
{
    while(1)
    {
        std::cout << "3 s." << std::endl;
        boost::this_thread::sleep(boost::posix_time::seconds(3)); //This emulates the "blocking" in this example.
    }
}

void test_case_io_service(void)
{
    timelog.begin("logfile_sleep_tester.log");
    timelog.write(std::ostringstream().flush() << "timer_ms\tsleeptime\tcpu_load" << std::endl);

    boost::asio::io_service io_service;
    boost::asio::deadline_timer timer(io_service, boost::posix_time::seconds(0));
    timer.async_wait(boost::bind(time_based_function,boost::asio::placeholders::error, &timer));

    //boost::thread self_blocking_thread(self_blocking_function);

    io_service.run();


}
