#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include "HardwareSerial/HardwareSerial.h"
#include "AP_GPS/GPS.h"
#include "AP_GPS/AP_GPS.h"
#include "ARDUINO/Stream.h"
#include "Logging/Logging.h"
#include "Sensors/i_GPS.h"


class gps_module: public AP_GPS_MTK16, public iGPS
{
    public:
        gps_module();
        ~gps_module();
        iGPS* get_gps_ptr(void ) {return(this);};

//        boost::signals2::signal<void (void)>  signal_newdata;
        std::string getString(void);

         void position_ecf_m(float* north, float* east, float* down);
         void velocity_ned_ms(float* north, float* east, float* down);
         uint32_t time(void);                      ///< GPS time (milliseconds from epoch)
         uint32_t date(void);                      ///< GPS date (FORMAT TBD)

    protected:
    private:
        HardwareSerial gps_usb_serial;
        Logging gpslog;

        boost::thread gps_thread;
        bool thread_running;

        void loop(void);

        void gps_log(void) { gpslog.data(this->getString()); }

};

#endif // GPS_MODULE_H
