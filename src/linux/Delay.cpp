#include <boost/thread/thread.hpp>

#include "Delay.h"

void delay(unsigned long ms)
{
    boost::this_thread::sleep(boost::posix_time::microseconds(ms*1000));
}


void delayMicroseconds(unsigned int us)
{
    boost::this_thread::sleep(boost::posix_time::microseconds(us));
}
