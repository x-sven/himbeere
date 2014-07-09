#ifndef IMU_MODULE_H
#define IMU_MODULE_H

#include "Drotek10dof/Drotek10dof.h"
#include "Logging/Logging.h"
#include "Sensors/i_IMU.h"


class imu_module: public Drotek10dof
{
    public:
        imu_module();
        ~imu_module();
        iIMU* get_imu_ptr(void ) {return(this);};
        iMAG* get_mag_ptr(void ) {return(this);}; // ugly, here!?
        iBARO* get_baro_ptr(void ) {return(this);}; // ugly, here!?
    protected:
    private:
        Logging imulog;

        void imu_log(void) { imulog.data(Drotek10dof::getString()); }
};

#endif // IMU_MODULE_H
