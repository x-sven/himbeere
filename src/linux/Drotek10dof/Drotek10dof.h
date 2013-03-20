#ifndef DROTEK10DOF_H
#define DROTEK10DOF_H

#include <boost/thread.hpp>
#include <boost/signal.hpp>

#include <I2Cdev.h>
#include <MPU6050/MPU6050.h>
#include <HMC5883L/HMC5883L.h>
#include <MS561101/MS561101BA.h>

class Drotek10dof
{
    public:
        Drotek10dof();
        ~Drotek10dof();
        void begin(uint16_t frequency = 100, uint8_t sched_priority = 1, uint8_t sched_policy = SCHED_FIFO);
        //http://www.kernel.org/doc/man-pages/online/pages/man2/sched_setscheduler.2.html

        void end(void);

        void connect_imu_callback(boost::function<void (void)>func);
        void connect_baromag_callback(boost::function<void (void)>func);
        float getTimingAverage(void); //returns calculation time average w.r.t. a second

        void getIMUConfigString(const char* prefix);


        int16_t ax, ay, az;
        int16_t gx, gy, gz;
        int16_t mx, my, mz;
        int32_t pressure, temp;

    protected:
    private:
        MPU6050 *imu;
        HMC5883L *mag;
        MS561101BA *baro;

        void loop(void);

        boost::thread imu_thread;
        bool thread_running;
        uint16_t m_frequency;

        boost::signal<void (void)>  signal_imudata;
        boost::signal<void (void)>  signal_baromagdata;

        long     m_imu_timing_buffer; //milliseconds
        uint16_t m_imu_timing_counter;
        float    m_timing_average;    //milliseconds



};

#endif // DROTEK10DOF_H
