#ifndef __IMU_H__
#define __IMU_H__

#include <boost/signals2.hpp>

class iIMU
{
    public:
        iIMU(){;};
        virtual ~iIMU(){};

        virtual void getAccel_mss(float *ax, float *ay, float *az) = 0;
        virtual void getRates_rads(float *rx, float *ry, float *rz) = 0;

        boost::signals2::signal<void (void)>  signal_imudata;

    protected:
    private:

};

#endif // __IMU_H__
