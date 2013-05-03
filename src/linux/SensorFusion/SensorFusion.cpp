#include "SensorFusion.h"

#include "../Sensors/i_IMU.h"


SensorFusion::SensorFusion()
{
    BackgroundAction::signal_trigger.connect( boost::bind(&ECFClass::update, this) );
}

void SensorFusion::update(void)
{
//    float acc[3] = {0., 0., 9.81};
//    float gyro[3]= {0., 0., 0.};
//    float mag[3] = {0., 0., 0.};
//
//    iIMU::getAccel_mss (&acc[0], &acc[1], &acc[2]);
//    iIMU::getRates_rads(&gyro[0], &gyro[1], &gyro[2]);

//    imu10dof.getScaledMAG(&mag[0], &mag[1], &mag[2]);

    BackgroundAction::update();
}

SensorFusion::~SensorFusion()
{
    //dtor
}
