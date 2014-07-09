#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include "HardwareSerial/HardwareSerial.h"
#include "cGPS/cGPS.h"
#include "Logging/Logging.h"


class gps_module
{
    public:
        gps_module();
        ~gps_module();

        cGPS *gps;

    protected:
    private:
        HardwareSerial *gps_usb_serial;
        Logging gpslog;

        void gps_log(void) { gpslog.data(gps->getString()); }

};

#endif // GPS_MODULE_H
