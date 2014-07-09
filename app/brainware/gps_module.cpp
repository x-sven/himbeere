#include <boost/signal.hpp>
#include <boost/bind.hpp>


#include "gps_module.h"

gps_module::gps_module()
{
    gps_usb_serial = NULL;
    gps_usb_serial = new HardwareSerial("/dev/ttyUSB0");
    gps = new cGPS(gps_usb_serial);

    std::stringstream headline;
    headline << "latitude" << "\t"
             << "longitude" << "\t"
             << "altitude" << "\t"
             << "ground_speed" << "\t"
             << "ground_course" << "\t"
             << "num_sats" << "\t"
             << "fix" << "\t"
             << "hdop" << "\t"
             << "time"  << "\t"
             << "date" << "\t"
             << std::endl;
    gpslog.header(headline.str());
    gpslog.begin("logfile_gps-mtk.log");

    gps->signal_newdata.connect ( boost::bind(&gps_module::gps_log,this) );

    gps_usb_serial->begin(38400);
}

gps_module::~gps_module()
{
//    gps->signal_newdata.disconnect ( boost::bind(&gps_module::gps_log,this) );
    gpslog.end();

    if(NULL != gps_usb_serial)
    {
        gps_usb_serial->end();
        delete(gps_usb_serial);
        gps_usb_serial = NULL;
    }
}
