#ifndef DATALINK_MODULE_H
#define DATALINK_MODULE_H

// includes from: src/linux/
#include "Delay.h"
#include "millis.h"
#include "DataLink/cDataLink.h"

#include "Sensors/i_IMU.h"
#include "Sensors/i_MAG.h"
#include "Sensors/i_BARO.h"
#include "SensorFusion/SensorFusion.h"
#include "cGPS/cGPS.h"

class datalink_module: public cDataLink
{
    public:
        /** Default constructor */
        datalink_module();
        datalink_module(iIMU*, iMAG*, iBARO*, cGPS*, SensorFusion*);
        /** Default destructor */
        virtual ~datalink_module();
    protected:
    private:

        boost::thread the_thread;
        bool thread_running;
        void start(void);
        void loop(void);
        void send_messages(void);

        iIMU* m_imu;
        iMAG* m_mag;
        iBARO* m_baro;
        cGPS* m_gps;
        SensorFusion* m_sf;
};

#endif // DATALINK_MODULE_H
