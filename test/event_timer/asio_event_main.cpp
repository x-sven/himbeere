#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

void time_based_function(const boost::system::error_code &, boost::asio::deadline_timer* timer) // = IMU LOOP
{
    timer->expires_from_now(boost::posix_time::seconds(1));

    std::cout << "1 s." << std::endl; //We do the calcualtions here...

    timer->async_wait(boost::bind(time_based_function,boost::asio::placeholders::error, timer));
}

void self_blocking_function(void) // = GPS LOOP
{
    while(1)
    {
        std::cout << "3 s." << std::endl;
        boost::this_thread::sleep(boost::posix_time::seconds(3)); //This emulates the "blocking" in this example.
    }
}

int main()
{
    boost::asio::io_service io_service;
    boost::asio::deadline_timer timer(io_service, boost::posix_time::seconds(0));
    timer.async_wait(boost::bind(time_based_function,boost::asio::placeholders::error, &timer));

    boost::thread self_blocking_thread(self_blocking_function);

    io_service.run();
}
