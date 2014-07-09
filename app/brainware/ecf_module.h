#ifndef ECF_MODULE_H
#define ECF_MODULE_H

#include "SensorFusion/SensorFusion.h"
#include "Logging/Logging.h"
#include "Sensors/i_IMU.h"

class ecf_module
{
    public:
        ecf_module();
        ecf_module(iIMU*, iMAG*, gps_module* m_the_gps);
        SensorFusion* get_sf_ptr(){return(&ecf);};
        ~ecf_module();
    protected:
    private:
        SensorFusion ecf;
        Logging ecflog;
        void ecf_logging(void);
        void ecf_log(void) { ecflog.data(ecf.getString()); }

};

#endif // ECF_MODULE_H
