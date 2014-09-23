#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
namespace pt = boost::posix_time;

#include "SensorFusion.h"

SensorFusion::SensorFusion()
{
    BackgroundAction::signal_trigger.connect( boost::bind(&ECFClass::update, this) );
}

std::string SensorFusion::getString(void)
{
    std::stringstream ss;
    const float C_RAD2DEG = 57.2957795131;

    ss  << get_euler_angles_rad().toDeg().roll << "\t"
        << get_euler_angles_rad().toDeg().pitch << "\t"
        << get_euler_angles_rad().toDeg().yaw << "\t"
        << get_CorrectedRate_rads(0)*C_RAD2DEG << "\t" //Orientation!!!
        << get_CorrectedRate_rads(1)*C_RAD2DEG << "\t"
        << get_CorrectedRate_rads(2)*C_RAD2DEG << "\t"
        << get_Acceleration_mss(0) << "\t"
        << get_Acceleration_mss(1) << "\t"
        << get_Acceleration_mss(2)  << "\t"
        << endl;

    return(ss.str());
}


void SensorFusion::register_imu(iIMU* _imu)
{
    m_the_imu = _imu;
}

void SensorFusion::register_mag(iMAG* _mag)
{
    m_the_mag = _mag;
}

void SensorFusion::register_gps(iGPS* _gps)
{
    m_the_gps = _gps;
}
//***************************************
void SensorFusion::imu_update(void)
{
    pt::time_duration difftime;
    static pt::ptime last_update = pt::microsec_clock::local_time();

    float acc[3] = {0., 0., 9.81};
    float gyro[3]= {0., 0., 0.};

    m_the_imu->getAccel_mss(&acc[0], &acc[1], &acc[2]);
    m_the_imu->getRates_rads(&gyro[0], &gyro[1], &gyro[2]);
    //Orientation!?

    set_AccelVector_mss(acc[0], acc[1], acc[2]);
    set_GyroVector_rads(gyro[0], gyro[1], gyro[2]);

    difftime    = pt::microsec_clock::local_time() - last_update;
    last_update = pt::microsec_clock::local_time();

    G_Dt = difftime.total_microseconds()/1.e+6;
    update();
    signal_newdata();
}

void SensorFusion::mag_update(void)
{
    float mag[3] = {0., 0., 0.};

    m_the_mag->getField_Gauss(&mag[0], &mag[1], &mag[2]);
    //Orientation!?
    set_MagVector_Gauss(mag[0], mag[1], mag[2]);
}

void SensorFusion::gps_update(void)
{
    float vel_north = 0.;
    float vel_east  = 0.;
    float vel_down  = 0.;

    m_the_gps->velocity_ned_ms(&vel_north, &vel_east, &vel_down);
    set_speed_msdeg(vel_north, vel_east, vel_down);

//    float gps_heading_deg = 0.01745329252*(ground_course * 0.01); // in rad
//    float gps_speed_cm_s   = ground_speed * 0.01;                 // in meter
//
//    _velocity_north = gps_speed * cos(gps_heading);
//    _velocity_east  = gps_speed * sin(gps_heading);
//
//    // no good way to get descent rate
//    _velocity_down  = 0;
}


SensorFusion::~SensorFusion()
{
    //dtor
}
