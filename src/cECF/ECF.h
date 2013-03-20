#ifndef _ECF_H_INCLUDED
#define _ECF_H_INCLUDED

#include <inttypes.h>
#include "st_Euler.h"

#define ToRad(x) (x*0.01745329252)  // *pi/180
#define ToDeg(x) (x*57.2957795131)  // *180/pi

#define USE_GPS 1
/*Min Speed Filter for Yaw drift Correction*/
#define SPEEDFILT 2 // in m/s; >1 use min speed filter for yaw drift cancellation, 0=do not use speed filter
//OUTPUTMODE=1 will print the corrected data, 0 will print uncorrected data of the gyros (with drift), 2 will print accelerometer only data
//#define OUTPUTMODE 2


using namespace std;


class ECFClass
{

public:

  ECFClass(void); //Constructor
  void set_GyroVector_rads(float x, float y, float z);
  void set_AccelVector_mss(float x, float y, float z);
  float get_Acceleration_mss(uint8_t axis);
  float get_GyroRate_rads(uint8_t axis);
  float get_CorrectedRate_rads(uint8_t axis);
  void set_speed_msdeg(float ground_speed, float ground_course, float speed_3d);
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


  float ECF_Matrix[3][3];

  float G_Dt;    // Integration time (ECF algorithm)

private:
  void Matrix_update(void);
  void Normalize(void);
  void Drift_correction(void);
  void Accel_adjust(void);

  float Update_Matrix[3][3];
  float Temporary_Matrix[3][3];

  float Accel_Vector[3]; //Store the acceleration in a vector
  float Gyro_Vector[3];  //Store the gyros rates in a vector

  float Omega_Vector[3]; //Corrected Gyro_Vector data
  float Omega_P[3];      //Omega Proportional correction
  float Omega_I[3];      //Omega Integrator
  float Omega[3];        //Turn rate for accel adjustment

  float errorRollPitch[3];
  float errorYaw[3];
  float errorCourse;
  float COGX; //Course overground X axis
  float COGY; //Course overground Y axis

  float f_ground_speed;
  float f_ground_course;
  float f_speed_3d;

  const float f_g_const;

    float Kp_ROLLPITCH;
    float Ki_ROLLPITCH;
    float Kp_YAW;
    float Ki_YAW;

};

#endif //_ECF_H_INCLUDED



