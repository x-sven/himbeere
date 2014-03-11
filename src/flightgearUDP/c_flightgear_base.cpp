#if defined(_WIN32) || defined(WIN32)
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <string.h>

#include "c_flightgear_base.hpp"


c_flightgear_base::c_flightgear_base()
{

    memset((void*)&FGData, 0, sizeof(FGData));

    FGData.version = FG_NET_FDM_VERSION;
    FGData.version = htonl(FGData.version);
}

c_flightgear_base::~c_flightgear_base()
{
    //dtor
}


// The function htond is defined this way due to the way some
// processors and OSes treat floating point values.  Some will raise
// an exception whenever a "bad" floating point value is loaded into a
// floating point register.  Solaris is notorious for this, but then
// so is LynxOS on the PowerPC.  By translating the data in place,
// there is no need to load a FP register with the "corruped" floating
// point value.  By doing the BIG_ENDIAN test, I can optimize the
// routine for big-endian processors so it can be as efficient as
// possible
void c_flightgear_base::htond (double &x)
{
    int    *Double_Overlay;
    int     Holding_Buffer;

    Double_Overlay = (int *) &x;
    Holding_Buffer = Double_Overlay [0];

    Double_Overlay [0] = htonl (Double_Overlay [1]);
    Double_Overlay [1] = htonl (Holding_Buffer);
}

void c_flightgear_base::htonf (float &x)
{
    int    *Float_Overlay;

    Float_Overlay = (int *) &x;
    Float_Overlay[0] = htonl(Float_Overlay[0]);
}

void c_flightgear_base::set_position_wgs84(double lat_rad, double lon_rad, double alt_m, float agl)
{
    FGData.latitude    = lat_rad;
    FGData.longitude   = lon_rad;
    FGData.altitude    = alt_m;
    FGData.agl         = agl;

    htond(FGData.longitude);
    htond(FGData.latitude);
    htond(FGData.altitude);
    htonf(FGData.agl);
}

void c_flightgear_base::set_attiude_euler(float phi_rad, float theta_rad, float psi_rad)
{
    FGData.phi     = phi_rad;
    FGData.theta   = theta_rad;
    FGData.psi     = psi_rad;

    htonf(FGData.phi);
    htonf(FGData.theta);
    htonf(FGData.psi);
}

FGNetFDM* c_flightgear_base::get_FGData()
{
// FIXME (ft14sl#1#): Not thread safe!?
     return(&FGData);
}
