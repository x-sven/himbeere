#ifndef C_FLIGHTGEAR_BASE_H
#define C_FLIGHTGEAR_BASE_H

#include "i_flightgear_sender.hpp"

class c_flightgear_base: public i_flightgear_sender
{
public:
    c_flightgear_base();
    virtual ~c_flightgear_base();

    void set_position_wgs84(double lat_rad, double lon_rad, double alt_m, float agl = 0);
    void set_position_ned(float x_m, float y_m, float z_m, double ref_lat_rad, double ref_lon_rad, double ref_alt_m);

    void set_attiude_euler(float phi_rad, float theta_rad, float psi_rad);
    void set_attiude_quaternion(float q0, float q1, float q2, float q3);


    FGNetFDM* get_FGData();

protected:
    void htond (double &x);
    void htonf (float &x);

    const double C_WGS84_ECC_2;   /*numerical eccentricity of WGS 84*/
    const double C_WGS84_a;		  /* WGS-84 semimajor axis (m) */
private:

    FGNetFDM FGData;
};

#endif // C_FLIGHTGEAR_BASE_H
