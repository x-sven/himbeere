#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <signal.h>
#include <time.h>

timespec ts_beg, ts_end;

bool execute = true;

void trap(int signal)
{
    execute = false;
}

void event_loop(void)
{
    #define frequency 10 //Hz
    int loopcount = 0;

    boost::posix_time::time_duration interval(boost::posix_time::microseconds(1000000 / frequency));
    boost::posix_time::ptime timer = boost::posix_time::microsec_clock::local_time() + interval;

    boost::this_thread::sleep(timer - boost::posix_time::microsec_clock::local_time());

    clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
    while(execute && loopcount < 500)
    {
        std::cout << loopcount  << std::endl; //We do the calcualtions here...
        for(int ii=0; ii<10000; ii++);

        loopcount++;
        timer = timer + interval;
        boost::this_thread::sleep(timer - boost::posix_time::microsec_clock::local_time());
    }
    clock_gettime(CLOCK_REALTIME, &ts_end);
    std::cout << "\n ...leaving thread after "
              << loopcount << " loops." << std::endl;

}



int main()
{

    std::cout << "3...";
    boost::thread event_thread(&event_loop);
    std::cout << "2...";

    signal(SIGINT, &trap);
    std::cout << "1...";

    std::cout << "go!\n";

    event_thread.join();

    std::cout << (float)( (ts_end.tv_sec - ts_beg.tv_sec)  + (ts_end.tv_nsec - ts_beg.tv_nsec) / 1e9)
              << " s." << std::endl;;

    std::cout << "\n SIGINT received, closing log!" << std::endl;

  return(0);
}
