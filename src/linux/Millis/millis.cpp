#include <stdint.h>
#include <iostream>
#include <sys/timeb.h>

#include "millis.h"

unsigned long millis()
{
    /*  Millisecond Timer*/
    /* http://www.cplusplus.com/forum/general/43203/*/
    timeb tb;
	ftime(&tb);
	unsigned long m = tb.millitm + (tb.time & 0xfffff) * 1000;

	return m;
}
