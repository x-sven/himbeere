#ifndef I_FLIGHTGEAR_SENDER_H
#define I_FLIGHTGEAR_SENDER_H


// Universal data
#include "universal.h"

// Select Flight Gear data structure
// #include "net_fdm_20.hxx"
#include "net_fdm_24.hxx" // Version 2.4.x

class i_flightgear_sender
{
    public:
        virtual void set_position_wgs84(double lat_rad, double lon_rad, double alt_m, float agl = 0)   = 0;
        virtual void set_attiude_euler(float phi_rad, float theta_rad, float psi_rad)                  = 0;

        virtual void update(void)                                                                      = 0;

    protected:
    private:

};

#endif // I_FLIGHTGEAR_SENDER_H
