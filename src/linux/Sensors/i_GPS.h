
#ifndef __GPS_H__
#define __GPS_H__

#include <boost/signal.hpp>

class iGPS
{
    public:
        iGPS(){;};
        virtual ~iGPS(){};

        boost::signal<void (void)>  signal_newdata;

        virtual void position_ecf_m(float* north, float* east, float* down)=0;
        virtual void velocity_ned_ms(float* north, float* east, float* down)=0;

    protected:
    private:


};

#endif // __GPS_H__
