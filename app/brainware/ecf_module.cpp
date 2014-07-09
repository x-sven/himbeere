#include "ecf_module.h"

ecf_module::ecf_module()
{
    ecf_logging();
    ecf.signal_newdata.connect( boost::bind(&ecf_module::ecf_log,this) );
}

void ecf_module::ecf_logging(void)
{
    std::stringstream headline;
    headline << "phi_deg" << "\t"
             << "theta_deg" << "\t"
             << "psi_deg" << "\t"
             << "p_deg_s" << "\t"
             << "q_deg_s" << "\t"
             << "r_deg_s" << "\t"
             << "ax_ms2" << "\t"
             << "ay_ms2" << "\t"
             << "az_ms2"  << "\t"
             << std::endl;
    ecflog.header(headline.str());
    ecflog.begin("logfile_ecf.log", 5); // prescale = 5 -> 20Hz);
}

ecf_module::ecf_module(iIMU* p_imu, iMAG* p_mag, gps_module* p_gps)
{
    if(NULL != p_imu)
    {
        ecf.register_imu(p_imu);
        p_imu->signal_imudata.connect(boost::bind(&SensorFusion::imu_update, &ecf) );
    }

    if(NULL != p_mag)
    {
        ecf.register_mag(p_mag);
        p_mag->signal_magdata.connect(boost::bind(&SensorFusion::mag_update, &ecf) );
    }
//    if(NULL != p_gps)
//    {
        m_the_gps=p_gps;
//    }

    ecf_logging();
    ecf.signal_newdata.connect( boost::bind(&ecf_module::ecf_log,this) );
}

ecf_module::~ecf_module()
{
    ecflog.end();
}
