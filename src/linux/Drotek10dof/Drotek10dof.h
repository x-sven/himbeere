#ifndef DROTEK10DOF_H
#define DROTEK10DOF_H

#include <boost/thread.hpp>
#include <boost/signal.hpp>
#include <boost/thread/mutex.hpp>

#include <I2Cdev.h>
#include <MPU6050/MPU6050.h>
#include <HMC5883L/HMC5883L.h>
#include <MS561101/MS561101BA.h>

#include "Sensors/i_IMU.h"
#include "Sensors/i_MAG.h"
#include "Sensors/i_BARO.h"

class Drotek10dof:
    public iIMU,
    public iMAG,
    public iBARO
{
    public:
        Drotek10dof(std::string _device = "/dev/i2c-1");
        ~Drotek10dof();
        void begin(uint16_t frequency = 100, uint8_t sched_priority = 30, uint8_t sched_policy = SCHED_FIFO);
        //http://www.kernel.org/doc/man-pages/online/pages/man2/sched_setscheduler.2.html

        void end(void);

        float getTimingAverage(void); //returns calculation time average w.r.t. a second

        std::string getConfigString(const char* prefix);
        std::string getString(void);

        void getScaledIMU(float *ax, float *ay, float *az, float *rx, float *ry, float *rz);
        void getScaledMAG(float *mx, float *my, float *mz);
        void getScaledBaro(float *p, float *T);

        void getScaledMAG(float *mx, float *my, float *mz, boost::posix_time::ptime *ptime);
        void getScaledBaro(float *_p, float *_T, boost::posix_time::ptime *ptime);
        void getScaledIMU(float *ax, float *ay, float *az, float *rx, float *ry, float *rz, boost::posix_time::ptime *ptime);

        void run_mag_calibration(void);
        void run_mag_calibration(uint16_t runtime); //seconds

        //iIMU
        void getAccel_mss (float *ax, float *ay, float *az) { float dummy; getScaledIMU(ax, ay, az, &dummy, &dummy, &dummy); };
        void getRates_rads(float *rx, float *ry, float *rz) { float dummy; getScaledIMU(&dummy, &dummy, &dummy, rx, ry, rz); };
        //iMAG
        void getField_Gauss(float *mx, float *my, float *mz){ getScaledMAG(mx, my, mz);};
        //iBARO
        void getPressure_pa(float *p)       { float dummy; getScaledBaro(p, &dummy);};
        void getTemperature_deg(float *T)   { float dummy; getScaledBaro(&dummy, T);};

    protected:
    private:

        MPU6050 *imu;
        HMC5883L *mag;
        MS561101BA *baro;

        boost::posix_time::ptime time_imu;
        int16_t ax, ay, az;
        int16_t gx, gy, gz;

        boost::posix_time::ptime time_mag;
        int16_t mx, my, mz;

        boost::posix_time::ptime time_baro;
        int32_t pressure, temp;

        void loop(void);

        boost::thread imu_thread;
        bool thread_running;
        uint16_t m_frequency;

        long     m_imu_timing_buffer; //milliseconds
        uint16_t m_imu_timing_counter;
        float    m_timing_average;    //milliseconds

        void mag_calibration(void);
        void mag_calibration(int16_t _mx, int16_t _my, int16_t _mz);

        int16_t mag_min[3];
        int16_t mag_max[3];
        float mag_ofs[3];
        bool mag_calibration_running;
        void mag_calibration_timer(uint16_t runtime);
        boost::thread calibration_timer;
        boost::mutex mutex;
};

#endif // DROTEK10DOF_H
