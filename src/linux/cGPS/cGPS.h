#ifndef _C_GPS_H_
#define _C_GPS_H_

#include <string>
#include <boost/signal.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>

// includes from: src
#include "AP_GPS/AP_GPS.h"
#include "ARDUINO/Stream.h"
#include "Sensors/i_GPS.h"


class cGPS: public AP_GPS_MTK16, public iGPS
{
    public:

        cGPS(Stream* s);
        ~cGPS();

        boost::signal<void (void)>  signal_newdata;
        std::string getString(void);

    protected:
    private:
        boost::thread gps_thread;
        bool thread_running;

        void loop(void);
};

#endif // _C_GPS_H_
