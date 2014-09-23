#ifndef SENSORFUSION_H
#define SENSORFUSION_H

#include "cECF/ECF.h"
#include "BackgroundAction/BackgroundAction.h"

#include "Sensors/i_IMU.h"
#include "Sensors/i_MAG.h"
#include "Sensors/i_GPS.h"


class SensorFusion: public ECFClass, public BackgroundAction
{
    public:
        SensorFusion();
        virtual ~SensorFusion();

        boost::signal<void (void)>  signal_newdata;
        std::string getString(void);

        void register_imu(iIMU*);
        void register_mag(iMAG*);
        void register_gps(iGPS*);

        void imu_update(void);
        void mag_update(void);
        void gps_update(void);

        void update(void){BackgroundAction::update();};
    protected:
    private:
        iIMU* m_the_imu;
        iMAG* m_the_mag;
        iGPS* m_the_gps;

};

#endif // SENSORFUSION_H
