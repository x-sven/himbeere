#include <stdint.h>
#include <iostream>
//#include <sys/timeb.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "millis.h"

unsigned long millis()
{
//    /*  Millisecond Timer*/
//    /* http://www.cplusplus.com/forum/general/43203/*/
//    timeb tb;
//	ftime(&tb);
//	unsigned long m = tb.millitm + (tb.time & 0xfffff) * 1000;
//
//	return m;
    static const boost::posix_time::ptime time_0 = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration time_now = boost::posix_time::microsec_clock::local_time()-time_0;
    return ((unsigned long)time_now.total_milliseconds());
}
