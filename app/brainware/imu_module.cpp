#include "imu_module.h"

imu_module::imu_module():
    Drotek10dof("/dev/i2c-1")
{
    std::stringstream headline;
    headline << "ax" << "\t"
             << "ay" << "\t"
             << "az" << "\t"
             << "gx" << "\t"
             << "gy" << "\t"
             << "gz" << "\t"
             << "mx" << "\t"
             << "my" << "\t"
             << "mz" << "\t"
             << "pressure" << "\t"
             << "temp" << "\t"
             << "runtime_average_ms" << "\t"
             << std::endl;
    imulog.header(headline.str());
    imulog.begin("logfile_drotek10.log", 10); // prescale = 10 -> 10Hz

    Drotek10dof::signal_imudata.connect ( boost::bind(&imu_module::imu_log,this) );

    Drotek10dof::begin(100,50,SCHED_FIFO);  //frequency = 100, sched_priority = 30, sched_policy = SCHED_FIFO
    Drotek10dof::getConfigString("# ");

}


imu_module::~imu_module()
{
    imulog.end();
    Drotek10dof::end();
}
