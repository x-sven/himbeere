// Origin: arduimu
// Released under Creative Commons License
// Code by Jordi Munoz and William Premerlani, Supported by Chris Anderson and Doug Weibel
// adopted by Sven Lorenz

#ifndef _ECF_H_INCLUDED
#define _ECF_H_INCLUDED

#include <inttypes.h>
#include <math.h>
#include "st_Euler.h"

#define ToRad(x) (x*0.01745329252f)  // *pi/180
#define ToDeg(x) (x*57.2957795131f)  // *180/pi

#define USE_GPS 1
/*Min Speed Filter for Yaw drift Correction*/
#define SPEEDFILT 2 // in m/s; >1 use min speed filter for yaw drift cancellation, 0=do not use speed filter
//OUTPUTMODE=1 will print the corrected data, 0 will print uncorrected data of the gyros (with drift), 2 will print accelerometer only data
//#define OUTPUTMODE 2

class ECFClass
{

public:

  ECFClass(void); //Constructor
  void set_GyroVector_rads(float x, float y, float z);
  void set_AccelVector_mss(float x, float y, float z);
  void set_MagVector_Gauss(float x, float y, float z);

  float get_Acceleration_mss(uint8_t axis);
  float get_GyroRate_rads(uint8_t axis);
  float get_CorrectedRate_rads(uint8_t axis);
  float get_speed_ms(uint8_t axis);

// NOTE (Sven#1#): Better to use other velocity representation here!?
//  void set_speed_msdeg(float ground_speed, float ground_course, float speed_3d);
  void set_speed_ms(float vel_north, float vel_east, float vel_down);

  void update(void);
  stEuler_t get_euler_angles_rad(void);
  stEuler_t get_euler_angles_from_acc_rad(void);
    void set_Kp_RollPitch(float k);
    float get_Kp_RollPitch(void);
    void set_Ki_RollPitch(float k);
    float get_Ki_RollPitch(void);
    void set_Kp_Yaw(float k);
    float get_Kp_Yaw(void);
    void set_Ki_Yaw(float k);
    float get_Ki_Yaw(void);


  float DCM_Matrix[3][3];

  float G_Dt;    // Integration time (ECF algorithm)

private:
    void Matrix_update(void);
    void Normalize(void);
    void Drift_correction(void);
    void Accel_adjust(void);

    float Update_Matrix[3][3];
    float Temporary_Matrix[3][3];

    float Accel_Vector[3]; //Store the acceleration in a vector (m/s2)
    float Gyro_Vector[3];  //Store the gyros rates in a vector (rad/s)
    float Mag_Vector[3];   //Store the magnetic field in a vector (Gauss)

    float Omega_Vector[3]; //Corrected Gyro_Vector data
    float Omega_P[3];      //Omega Proportional correction
    float Omega_I[3];      //Omega Integrator
    float Omega[3];        //Turn rate for accel adjustment

    float errorRollPitch[3];
    float errorYaw[3];

    float f_ground_speed_ms;    // horizontal speed in m/s
    float f_ground_course_deg;  // ground course angle in deg
    float f_speed_3d_ms;        // total speed in m/s, not from any sensor!

    float f_vel_north_ms;   // speed in north direction in m/s
    float f_vel_east_ms;    // speed in east direction in m/s
    float f_vel_down_ms;    // speed in down direction in m/s

    const float f_g_const;

    float f_declination; //Declination for magnetometer correction
    bool mag_enabled;

    float Kp_ROLLPITCH;
    float Ki_ROLLPITCH;
    float Kp_YAW;
    float Ki_YAW;

    float wrap_pi ( float r )
    {
        while ( r > M_PI ) {
            r -= 2 * M_PI;
        }
        while ( r <= -M_PI ) {
            r += 2 * M_PI;
        }
        return r;
    }
};

#endif //_ECF_H_INCLUDED



