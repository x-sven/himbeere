#if defined(_WIN32) || defined(WIN32)
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <string.h>
#include <math.h>

#include "c_flightgear_base.hpp"


c_flightgear_base::c_flightgear_base():
    C_WGS84_ECC_2(0.006694379992),
    C_WGS84_a(6378137.0)		  /* WGS-84 semimajor axis (m) */
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

void c_flightgear_base::set_position_ned(float x_m, float y_m, float z_m, double ref_lat_rad, double ref_lon_rad, double ref_alt_m)
{
    double buf = 1.0 - C_WGS84_ECC_2*sqrt(sin( ref_lat_rad ));
    double Rn = C_WGS84_a * ( 1.0 - C_WGS84_ECC_2 ) * pow( buf, (-1.5) );
    double Re = C_WGS84_a / sqrt( buf );

    double alt_rad = ref_alt_m - z_m;
    double lat_rad = ref_lat_rad + x_m/(Rn + ref_alt_m );
    double lon_rad = ref_lon_rad + y_m/(Re + ref_alt_m)/cos(ref_lat_rad);

    set_position_wgs84(lat_rad, lon_rad, alt_rad);
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

void c_flightgear_base::set_attiude_quaternion(float q0, float q1, float q2, float q3)
{
    float q02 = q0*q0;
    float q12 = q1*q1;
    float q22 = q2*q2;
    float q32 = q3*q3;

    float phi  = atan2(   2.0f * (q2*q3 + q0*q1), q02+q32-q12-q22 );
    float theta = asin( - 2.0f * (q1*q3 - q0*q2) );
    float psi  = atan2(   2.0f * (q1*q2 + q0*q3), q02+q12-q32-q22 );

    set_attiude_euler( phi, theta, psi);
}


FGNetFDM* c_flightgear_base::get_FGData()
{
// FIXME (ft14sl#1#): Not thread safe!?
    return(&FGData);
}
