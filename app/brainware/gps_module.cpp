#include <boost/signals2.hpp>
#include <boost/bind.hpp>


#include "gps_module.h"

#ifndef ToRad
    #define ToRad(x) (x*0.01745329252)  // *pi/180
#endif
#ifndef ToRad
    #define ToDeg(x) (x*57.2957795131)  // *180/pi
#endif

gps_module::gps_module():
    AP_GPS_MTK16( &gps_usb_serial ),
    gps_usb_serial("/dev/ttyUSB0")
{
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

    this->signal_newdata.connect ( boost::bind(&gps_module::gps_log,this) );

    gps_usb_serial.begin(38400);

    // GPS Initialization
    this->print_errors = true;
    this->init();

    // create thread
    thread_running = true;
    gps_thread = boost::thread( boost::bind(&gps_module::loop, this));

    struct sched_param thread_param;
    thread_param.sched_priority = 5;
    pthread_t threadID = (pthread_t) gps_thread.native_handle();

    int retcode;
    if ((retcode = pthread_setschedparam(threadID, SCHED_FIFO, &thread_param)) != 0)
    {
        errno = retcode;
        perror("pthread_setschedparam");
    }
}

void gps_module::loop(void)
{
    boost::posix_time::time_duration interval_10Hz(boost::posix_time::milliseconds(1000 / 10));
    boost::posix_time::ptime timer = boost::posix_time::microsec_clock::local_time() + interval_10Hz;

    while(thread_running)
    {
        this->update();

        if (this->new_data)
        {
            this->new_data = 0;
            signal_newdata();
        }
        boost::this_thread::sleep(timer - boost::posix_time::microsec_clock::local_time());
        timer += interval_10Hz; // update timer
    }
}

std::string gps_module::getString(void)
{
    std::stringstream ss;

    ss  << this->latitude << "\t"
        << this->longitude << "\t"
        << this->altitude << "\t"
        << this->ground_speed << "\t"
        << this->ground_course << "\t"
        << this->num_sats << "\t"
        << this->fix << "\t"
        << this->hdop << "\t"
        << this->GPS::time  << "\t"
        << this->GPS::date << "\t"
        << std::endl;
    return(ss.str());
}

void gps_module::position_ecf_m(float* north, float* east, float* down)
{

}


void gps_module::velocity_ned_ms(float* north, float* east, float* down)
{
    *north = this->ground_speed*0.01f *cos(ToRad(this->ground_course*0.01f));
    *east  = this->ground_speed*0.01f *sin(ToRad(this->ground_course*0.01f));
    *down  = fabs(ground_speed*0.01f) - this->speed_3d*0.01f;
}

gps_module::~gps_module()
{
//    gps->signal_newdata.disconnect ( boost::bind(&gps_module::gps_log,this) );
    thread_running = false;

    gpslog.end();

//    if(NULL != gps_usb_serial)
//    {
        gps_usb_serial.end();
//        delete(gps_usb_serial);
//        gps_usb_serial = NULL;
//    }
}
