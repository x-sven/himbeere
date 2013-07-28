#if !defined(__linux__)  && !defined(__APPLE__)
#include <Arduino.h>
#else
#include <iostream>
#endif

#include <math.h>
#include <inttypes.h>
#include "wiring.h" //constrain macro

#include "VectorMath.h"
#include "MatrixMath.h"
#include "ECF.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

ECFClass::ECFClass(void):f_g_const(9.81)
{

    for(uint8_t ii=0; ii<3; ii++)
    {
        Accel_Vector[ii] = 0.0;
        Gyro_Vector[ii]  = 0.0;
        Mag_Vector[ii] = 0.0;
        Omega_Vector[ii] = 0.0;
        Omega_P[ii] = 0.0;
        Omega_I[ii] = 0.0;
        Omega[ii]   = 0.0;
        errorRollPitch[ii] = 0.0;
        errorYaw[ii] = 0.0;
    }//for
    Accel_Vector[2] = -f_g_const;

    for(uint8_t jj=0; jj<3; jj++)
    {
        for(uint8_t kk=0; kk<3; kk++)
        {
            Update_Matrix[jj][kk] = 0.0;
            Temporary_Matrix[jj][kk] = 0.0;
            DCM_Matrix[jj][kk] = 0.0;
            if(jj == kk)
            {
                DCM_Matrix[jj][kk]=1.0;
            }//if
        }//for kk
    }//for jj

    G_Dt=0.01;
    f_ground_course = 0.0;
    f_ground_speed = 0.0;
    f_speed_3d = 0.0;
    f_declination = 0.0;
    mag_enabled = false;

    Kp_ROLLPITCH = -0.15;
    Ki_ROLLPITCH = -0.00010;
    Kp_YAW = -4; // was -0.5
    Ki_YAW = Kp_YAW/1.e+6;
}

/**************************************************/
void ECFClass::set_GyroVector_rads(float x, float y, float z)
{
    Gyro_Vector[0]= x;
    Gyro_Vector[1]= y;
    Gyro_Vector[2]= z;
}

/**************************************************/
void ECFClass::set_AccelVector_mss(float x, float y, float z)
{
    Accel_Vector[0]= x;
    Accel_Vector[1]= y;
    Accel_Vector[2]= z;
}

/**************************************************/
void ECFClass::set_MagVector_Gauss(float x, float y, float z)
{
    Mag_Vector[0]= x;
    Mag_Vector[1]= y;
    Mag_Vector[2]= z;

    mag_enabled = true;
}

/**************************************************/
float ECFClass::get_Acceleration_mss(uint8_t axis)
{
    if(axis < 3)
        return(this->Accel_Vector[axis]);
    else
        return(0.0);
}

/**************************************************/
float ECFClass::get_GyroRate_rads(uint8_t axis)
{
    if(axis < 3)
        return(this->Gyro_Vector[axis]);
    else
        return(0.0);
}

/**************************************************/
float ECFClass::get_CorrectedRate_rads(uint8_t axis)
{
    if(axis < 3)
        return(this->Omega_Vector[axis]);
    else
        return(0.0);
}

/**************************************************/
void ECFClass::set_speed_msdeg(float ground_speed, float ground_course, float speed_3d)
{
    f_ground_speed = ground_speed;
    f_ground_course = ground_course;
    f_speed_3d = speed_3d;
}


/**************************************************/
void ECFClass::update(void)
{

    Matrix_update();
    Normalize();
    Drift_correction();
}

/**************************************************/

void ECFClass::Matrix_update(void)
{
    Vector_Add(&Omega[0], &Gyro_Vector[0], &Omega_I[0]);  //adding Integrator term
    Vector_Add(&Omega_Vector[0], &Omega[0], &Omega_P[0]); //adding proportional term
#if USE_GPS
 //   Accel_adjust();    //Remove centrifugal acceleration.
#endif /*USE_GPS*/

    Update_Matrix[0][0]= 0.0;
    Update_Matrix[0][1]=-G_Dt*Omega_Vector[2];//-z
    Update_Matrix[0][2]= G_Dt*Omega_Vector[1];//y
    Update_Matrix[1][0]= G_Dt*Omega_Vector[2];//z
    Update_Matrix[1][1]= 0.0;
    Update_Matrix[1][2]=-G_Dt*Omega_Vector[0];//-x
    Update_Matrix[2][0]=-G_Dt*Omega_Vector[1];//-y
    Update_Matrix[2][1]= G_Dt*Omega_Vector[0];//x
    Update_Matrix[2][2]= 0.0;

    Matrix_Multiply(Temporary_Matrix, DCM_Matrix, Update_Matrix); //c=a*b

    for(int x=0; x<3; x++) //Matrix Addition (update)
    {
        for(int y=0; y<3; y++)
        {
            DCM_Matrix[x][y]+=Temporary_Matrix[x][y];
        }
    }
}
/**************************************************/
void ECFClass::Normalize(void)
{
    float error=0;
    float temporary[3][3];
    float renorm=0;
    uint8_t problem=FALSE;

    error= -Vector_Dot_Product(&DCM_Matrix[0][0],&DCM_Matrix[1][0])*.5; //eq.19

    Vector_Scale(&temporary[0][0], &DCM_Matrix[1][0], error); //eq.19
    Vector_Scale(&temporary[1][0], &DCM_Matrix[0][0], error); //eq.19

    Vector_Add(&temporary[0][0], &temporary[0][0], &DCM_Matrix[0][0]);//eq.19
    Vector_Add(&temporary[1][0], &temporary[1][0], &DCM_Matrix[1][0]);//eq.19

    Vector_Cross_Product(&temporary[2][0],&temporary[0][0],&temporary[1][0]); // c= a x b //eq.20

    renorm= Vector_Dot_Product(&temporary[0][0],&temporary[0][0]);
    if (renorm < 1.5625f && renorm > 0.64f)
    {
        renorm= .5 * (3-renorm);                                                 //eq.21
    }
    else if (renorm < 100.0f && renorm > 0.01f)
    {
        renorm= 1. / sqrt(renorm);
#if PRINT_DEBUG != 0
        Serial.print("!!!SQT:1,RNM:");
        Serial.print (renorm);
        Serial.print (",ERR:");
        Serial.print (error);
#if USE_GPS
        Serial.print (",TOW:");
        Serial.print (iTOW);
#endif /*USE_GPS*/
        Serial.println("***");
#endif
    }
    else
    {
        problem = TRUE;
#if PRINT_DEBUG != 0
        Serial.print("!!!PRB:1,RNM:");
        Serial.print (renorm);
        Serial.print (",ERR:");
        Serial.print (error);
#if USE_GPS
        Serial.print (",TOW:");
        Serial.print (iTOW);
#endif /*USE_GPS*/
        Serial.println("***");
#endif
    }
    Vector_Scale(&DCM_Matrix[0][0], &temporary[0][0], renorm);

    renorm= Vector_Dot_Product(&temporary[1][0],&temporary[1][0]);
    if (renorm < 1.5625f && renorm > 0.64f)
    {
        renorm= .5 * (3-renorm);                                                 //eq.21
    }
    else if (renorm < 100.0f && renorm > 0.01f)
    {
        renorm= 1. / sqrt(renorm);
#if PRINT_DEBUG != 0
        Serial.print("!!!SQT:2,RNM:");
        Serial.print (renorm);
        Serial.print (",ERR:");
        Serial.print (error);
#if USE_GPS
        Serial.print (",TOW:");
        Serial.print (iTOW);
#endif /*USE_GPS*/
        Serial.println("***");
#endif
    }
    else
    {
        problem = TRUE;
#if PRINT_DEBUG != 0
        Serial.print("!!!PRB:2,RNM:");
        Serial.print (renorm);
        Serial.print (",ERR:");
        Serial.print (error);
#if USE_GPS
        Serial.print (",TOW:");
        Serial.print (iTOW);
#endif /*USE_GPS*/
        Serial.println("***");
#endif
    }
    Vector_Scale(&DCM_Matrix[1][0], &temporary[1][0], renorm);

    renorm= Vector_Dot_Product(&temporary[2][0],&temporary[2][0]);
    if (renorm < 1.5625f && renorm > 0.64f)
    {
        renorm= .5 * (3-renorm);                                                 //eq.21
    }
    else if (renorm < 100.0f && renorm > 0.01f)
    {
        renorm= 1. / sqrt(renorm);
#if PRINT_DEBUG != 0
        Serial.print("!!!SQT:3,RNM:");
        Serial.print (renorm);
        Serial.print (",ERR:");
        Serial.print (error);
#if USE_GPS
        Serial.print (",TOW:");
        Serial.print (iTOW);
#endif /*USE_GPS*/
        Serial.println("***");
#endif
    }
    else
    {
        problem = TRUE;
#if PRINT_DEBUG != 0
        Serial.print("!!!PRB:3,RNM:");
        Serial.print (renorm);
#if USE_GPS
        Serial.print (",TOW:");
        Serial.print (iTOW);
#endif /*USE_GPS*/
        Serial.println("***");
#endif
    }
    Vector_Scale(&DCM_Matrix[2][0], &temporary[2][0], renorm);

    if (problem)                  // Our solution is blowing up and we will force back to initial condition.  Hope we are not upside down!
    {
        DCM_Matrix[0][0]= 1.0f;
        DCM_Matrix[0][1]= 0.0f;
        DCM_Matrix[0][2]= 0.0f;
        DCM_Matrix[1][0]= 0.0f;
        DCM_Matrix[1][1]= 1.0f;
        DCM_Matrix[1][2]= 0.0f;
        DCM_Matrix[2][0]= 0.0f;
        DCM_Matrix[2][1]= 0.0f;
        DCM_Matrix[2][2]= 1.0f;
        problem = FALSE;
    }
}

/**************************************************/
void ECFClass::Drift_correction(void)
{
    //Compensation the Roll, Pitch and Yaw drift.
    static float Scaled_Omega_P[3];
    static float Scaled_Omega_I[3];
    float Accel_magnitude;
    float Accel_weight;
    float Integrator_magnitude;
    float errorCourse = 0.0;
    float Heading_X;
    float Heading_Y;
    //*****Roll and Pitch***************

    // Calculate the magnitude of the accelerometer vector
    Accel_magnitude = sqrt(Accel_Vector[0]*Accel_Vector[0] + Accel_Vector[1]*Accel_Vector[1] + Accel_Vector[2]*Accel_Vector[2]);
    Accel_magnitude = Accel_magnitude / f_g_const; // Scale to gravity.

    // Dynamic weighting of accelerometer info (reliability filter)
    // Weight for accelerometer info (<0.5G = 0.0, 1G = 1.0 , >1.5G = 0.0)
    Accel_weight = constrain(1. - 2.*abs(1. - Accel_magnitude),0.,1.);  //

    Vector_Cross_Product(&errorRollPitch[0],&Accel_Vector[0],&DCM_Matrix[2][0]); //adjust the ground of reference
    Vector_Scale(&Omega_P[0],&errorRollPitch[0],Kp_ROLLPITCH*Accel_weight);

    Vector_Scale(&Scaled_Omega_I[0],&errorRollPitch[0],Ki_ROLLPITCH*Accel_weight);
    Vector_Add(Omega_I,Omega_I,Scaled_Omega_I);

    //*****YAW***************
    if(mag_enabled)
    {
        float cos_pitch;

        if(1 > (DCM_Matrix[2][0]*DCM_Matrix[2][0]))
        {
            cos_pitch = sqrt(1-(DCM_Matrix[2][0]*DCM_Matrix[2][0]));
            // we are pointing straight up or down so don't update our
            // heading using the compass. Wait for the next iteration when
            // we hopefully will have valid values again.
            if(1.e-3 < fabs(cos_pitch))
            {
                // Tilt compensated magnetic field X component:
                Heading_X = Mag_Vector[0]*cos_pitch - Mag_Vector[1]*DCM_Matrix[2][1]*DCM_Matrix[2][0]/cos_pitch - Mag_Vector[2]*DCM_Matrix[2][2]*DCM_Matrix[2][0]/cos_pitch;
                // Tilt compensated magnetic field Y component: (Sven: I have the sign changed!)
                Heading_Y = Mag_Vector[2]*DCM_Matrix[2][1]/cos_pitch - Mag_Vector[1]*DCM_Matrix[2][2]/cos_pitch;

                errorCourse= -(DCM_Matrix[0][0]*Heading_Y) +(DCM_Matrix[1][0]*Heading_X);  //Calculating YAW error
                Vector_Scale(errorYaw,&DCM_Matrix[2][0],errorCourse); //Applys the yaw correction to the XYZ rotation of the aircraft, depeding the position.

                Vector_Scale(&Scaled_Omega_P[0],&errorYaw[0],Kp_YAW);
                Vector_Add(Omega_P,Omega_P,Scaled_Omega_P);//Adding  Proportional.

                Vector_Scale(&Scaled_Omega_I[0],&errorYaw[0],Ki_YAW);
                Vector_Add(Omega_I,Omega_I,Scaled_Omega_I);//adding integrator to the Omega_I

            }
        }
    }
    else  // Use GPS Ground course to correct yaw gyro drift
    {
        if(f_ground_speed>=SPEEDFILT)
        {
            Heading_X = cos(ToRad(f_ground_course));
            Heading_Y = sin(ToRad(f_ground_course));

            errorCourse= -(DCM_Matrix[0][0]*Heading_Y) +(DCM_Matrix[1][0]*Heading_X);  //Calculating YAW error
            Vector_Scale(errorYaw,&DCM_Matrix[2][0],errorCourse); //Applys the yaw correction to the XYZ rotation of the aircraft, depeding its attitude.

// FIXME (Sven#1#): Scale factor required, indicates missing normialization of "error"-effects!?
            Vector_Scale(&Scaled_Omega_P[0],&errorYaw[0],Kp_YAW/8.);
            Vector_Add(Omega_P,Omega_P,Scaled_Omega_P);//Adding  Proportional.

            Vector_Scale(&Scaled_Omega_I[0],&errorYaw[0],Ki_YAW/8.);
            Vector_Add(Omega_I,Omega_I,Scaled_Omega_I);//adding integrator to the Omega_I

        }
    }


    //  Here we will place a limit on the integrator so that the integrator cannot ever exceed half the saturation limit of the gyros
    Integrator_magnitude = sqrt(Vector_Dot_Product(Omega_I,Omega_I));
    if (Integrator_magnitude > ToRad(300))
    {
        Vector_Scale(Omega_I,Omega_I,0.5f*ToRad(300)/Integrator_magnitude);
#if PRINT_DEBUG != 0
        Serial.print("!!!INT:1,MAG:");
        Serial.print (ToDeg(Integrator_magnitude));
#if USE_GPS
        Serial.print (",TOW:");
        Serial.print (iTOW);
#endif /*USE_GPS*/
        Serial.println("***");
#endif
    }


}
/**************************************************/
#if USE_GPS
void ECFClass::Accel_adjust(void)
{
    float centrifugal_accel_vector[3];
    float vel_g[3];

    //assumtion: slip angle = 0
    vel_g[0] = f_ground_speed*cos(ToRad(f_ground_course));
    vel_g[1] = f_ground_speed*sin(ToRad(f_ground_course));
    vel_g[2] = sqrt(vel_g[0]*vel_g[0] + vel_g[1]*vel_g[1]) - f_speed_3d;

    Vector_Cross_Product(&centrifugal_accel_vector[0],&Omega[0],&vel_g[0]); // delta_acc = omega x V
    Vector_Add(&Accel_Vector[0], &Accel_Vector[0], &centrifugal_accel_vector[0]);// acc = acc + delta_acc
}
#endif /* USE_GPS */

/**************************************************/
stEuler_t ECFClass::get_euler_angles_rad(void)
{
    stEuler_t Euler;

    Euler.pitch = -asin(DCM_Matrix[2][0]);
    Euler.roll  =  atan2(DCM_Matrix[2][1],DCM_Matrix[2][2]);
    Euler.yaw   =  atan2(DCM_Matrix[1][0],DCM_Matrix[0][0]);

    return(Euler);
}

/**************************************************/
stEuler_t ECFClass::get_euler_angles_from_acc_rad(void)
{
    stEuler_t Euler;

    Euler.roll  = -atan2(Accel_Vector[1],-Accel_Vector[2]);    // atan2(acc_y,acc_z)
    Euler.pitch = -asin((Accel_Vector[0])/(double)-f_g_const);
    Euler.yaw   = 0;

    return(Euler);
}

/**************************************************/
void ECFClass::set_Kp_RollPitch(float k)
{
    if(0 < k)
        Kp_ROLLPITCH = k;
}
/**************************************************/
float ECFClass::get_Kp_RollPitch(void)
{
    return(Kp_ROLLPITCH);
}
/**************************************************/
void ECFClass::set_Ki_RollPitch(float k)
{
    if(0 < k)
        Ki_ROLLPITCH = k;
}
/**************************************************/
float ECFClass::get_Ki_RollPitch(void)
{
    return(Ki_ROLLPITCH);
}
/**************************************************/
void ECFClass::set_Kp_Yaw(float k)
{
    if(0 < k)
        Kp_YAW = k;
}
/**************************************************/
float ECFClass::get_Kp_Yaw(void)
{
    return(Kp_YAW);
}
/**************************************************/
void ECFClass::set_Ki_Yaw(float k)
{
    if(0 < k)
        Ki_YAW = k;
}
/**************************************************/
float ECFClass::get_Ki_Yaw(void)
{
    return(Ki_YAW);
}
