#ifndef C_FLIGHTGEAR_BASE_H
#define C_FLIGHTGEAR_BASE_H

#include "i_flightgear_sender.hpp"

class c_flightgear_base: public i_flightgear_sender
{
public:
    c_flightgear_base();
    virtual ~c_flightgear_base();

    void set_position_wgs84(double lat_rad, double lon_rad, double alt_m, float agl = 0);
    void set_attiude_euler(float phi_rad, float theta_rad, float psi_rad);

    FGNetFDM* get_FGData();

protected:
    void htond (double &x);
    void htonf (float &x);
private:

    FGNetFDM FGData;
};

#endif // C_FLIGHTGEAR_BASE_H
