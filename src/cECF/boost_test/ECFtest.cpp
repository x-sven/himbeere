#define BOOST_TEST_MODULE MyTest

#include <boost/test/included/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <cmath>
#include <ostream>
#include <inttypes.h>

//!< First read this: http://www.boost.org/doc/libs/1_53_0/libs/test/doc/html/utf/user-guide.html

//!< The following define is used to access privat methods
//!< without modifications of the included classes.
#define private public

using namespace std;

#include "wiring.h" //constrain macro

#include "st_Euler.h"
#include "ECF.h"

BOOST_AUTO_TEST_SUITE ( ECFtest )

BOOST_AUTO_TEST_CASE( stEulerTest )
{
    stEuler_t Euler;

    Euler.pitch = 30.0*M_PI/180.;
    BOOST_CHECK_CLOSE_FRACTION(Euler.toDeg().pitch, (float)30.0, (float)1e-2);
}

BOOST_AUTO_TEST_CASE( Euler_from_ECF )
{
    ECFClass ECF;

//    ECF.set_Kp_RollPitch(-0.015); // PPZ values
//    ECF.set_Ki_RollPitch(-0.00001);
//    ECF.set_Kp_Yaw(-0.9);
//    ECF.set_Ki_Yaw(-0.00005);
//
//    ECF.G_Dt = 0.02;

    //check initialization
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll ,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw  ,  (float)1e-5);

    ECF.set_AccelVector_mss(0.0, 0.0, -9.81);
    ECF.update();

    BOOST_CHECK_SMALL(ECF.get_Acceleration_mss(0),  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_Acceleration_mss(1),  (float)1e-5);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Acceleration_mss(2), (float)-9.81, (float)1e-5);

    BOOST_CHECK_CLOSE_FRACTION(ECF.errorRollPitch[0], 0.0, 1e-5);
    BOOST_CHECK_CLOSE_FRACTION(ECF.errorRollPitch[1], 0.0, 1e-5);
    BOOST_CHECK_CLOSE_FRACTION(ECF.errorRollPitch[2], 0.0, 1e-5);

    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().pitch,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().roll ,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().yaw  ,  (float)1e-5);

    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll ,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw  ,  (float)1e-5);

    ECF.set_AccelVector_mss(6.9367, 0.0, -6.9367);

#include <iostream>
    ofstream outfile;
    outfile.open("history_pitch.txt", ios_base::trunc);
    outfile<<"# Time\t"<<"pitch\t"<<"roll\t"<<"yaw"<<endl;
    for(int ii=0; ii<60*50; ii++)
    {
        ECF.update();
        outfile<<ii/50.<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().pitch<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().roll<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().yaw<<"\t"
        <<endl;
    }
    outfile.close();
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_from_acc_rad().toDeg().pitch, (float)45.0, (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().roll , (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().yaw  , (float)1e-5);

    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().pitch, (float)45.0, (float)0.5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll, (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw , (float)5.0);

    if (system(NULL))
    {
        /*int ret = */system ("gnuplot -persist plot_pitch.gnuplot");
    }


    ECF.set_AccelVector_mss(0.0, -6.9367, -6.9367);
    //ECF.set_speed_msdeg(10.0, 0.0, 10.0); // horz_speed, course_deg, abs_speed

    outfile.open("history_roll.txt", ios_base::trunc);
    outfile<<"# Time\t"<<"pitch\t"<<"roll\t"<<"yaw"<<endl;
    for(int ii=0; ii<60*50; ii++)
    {
        ECF.update();
        outfile<<ii/50.<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().pitch<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().roll<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().yaw<<"\t"
        <<endl;
    }
    outfile.close();

    //ECF is at pitch=45° --> reduce pitch increase roll...
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().pitch,  (float)1e-5);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_from_acc_rad().toDeg().roll , (float)45.0, (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().yaw  ,  (float)1e-5);

    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch, (float)0.5);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().roll , (float)45.0, (float)0.5);

    //BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw, (float)10.0); //for some reason it drifts a little bit

    if (system(NULL))
    {
        /*int ret = */system ("gnuplot -persist plot_roll.gnuplot");
    }

}

BOOST_AUTO_TEST_CASE( Euler_with_GPS )
{
    ECFClass ECF;
    ECF.G_Dt = 0.02;

    ofstream outfile;

    //check initialization
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll ,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw  ,  (float)1e-5);

    ECF.set_AccelVector_mss(0.0, 0.0, -9.81);
    ECF.update();

    BOOST_CHECK_SMALL(ECF.get_Acceleration_mss(0),  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_Acceleration_mss(1),  (float)1e-5);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Acceleration_mss(2), (float)-9.81, (float)1e-5);

    BOOST_CHECK_CLOSE_FRACTION(ECF.errorRollPitch[0], 0.0, 1e-5);
    BOOST_CHECK_CLOSE_FRACTION(ECF.errorRollPitch[1], 0.0, 1e-5);
    BOOST_CHECK_CLOSE_FRACTION(ECF.errorRollPitch[2], 0.0, 1e-5);

    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().pitch,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().roll ,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().yaw  ,  (float)1e-5);

    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll ,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw  ,  (float)1e-5);

    ECF.set_GyroVector_rads(0., 0., 10*M_PI/180.);
    for(int ii=0; ii<50*3; ii++) //3s
    {
        ECF.update();
    }
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch, (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll , (float)1e-5);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().yaw, (float)30.0, (float)1e-5);

    ECF.set_GyroVector_rads(0., 0., 0.);
    ECF.set_AccelVector_mss(0.0, 0.0, -9.81);
    ECF.set_speed_msdeg(10.0, 0.0, 10.0); // horz_speed, course_deg, abs_speed

    outfile.open("history_roll.txt", ios_base::trunc);
    outfile<<"# Time\t"<<"pitch\t"<<"roll\t"<<"yaw"<<endl;
    for(int ii=0; ii<60*50; ii++) // 60s
    {
        ECF.update();

        outfile<<ii/50.<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().pitch<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().roll<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().yaw<<"\t"
        <<endl;
    }
    outfile.close();

    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch, (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll , (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw  , (float)1e-2);

    if (system(NULL))
    {
        /*int ret = */system ("gnuplot -persist plot_roll.gnuplot");
    }
}


BOOST_AUTO_TEST_CASE( Matrix_update_test )
{

    ECFClass ECF;

    BOOST_CHECK_CLOSE_FRACTION(ECF.G_Dt, (float)0.01, (float)1e-6);
    ECF.G_Dt = 0.02;
    BOOST_CHECK_CLOSE_FRACTION(ECF.G_Dt, (float)0.02, (float)1e-6);


    ECF.set_GyroVector_rads( 0.0, 0.0, 0.0 );
    ECF.Matrix_update();
    ECF.Normalize();

    for (uint8_t ii=0; ii<3; ii++)
    {
        BOOST_CHECK_SMALL(ECF.get_GyroRate_rads(ii),  (float)1e-5);
        BOOST_CHECK_SMALL(ECF.get_CorrectedRate_rads(ii),  (float)1e-5);
        for (uint8_t jj=0; jj<3; jj++)
        {
            if(ii==jj)
                BOOST_CHECK_CLOSE_FRACTION(ECF.DCM_Matrix[ii][jj], (float)1.0, (float)1e-5);
            else
                BOOST_CHECK_SMALL(ECF.DCM_Matrix[ii][jj], (float)1e-5);
        }//for jj
    }//for ii

    ECF.set_GyroVector_rads( 0.0, 0.0, 10.0*M_PI/180.0 ); //10°/s yaw rate
    for(int ii=0; ii<1*5; ii++)
    {
        ECF.Matrix_update();
        ECF.Normalize();
    }//for ii
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll,  (float)1e-6);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch, (float)1e-6);
    BOOST_CHECK(fabs(ECF.get_euler_angles_rad().toDeg().yaw) < 10.0);

    ECF.set_GyroVector_rads( 0.0, 10.0*M_PI/180.0, 0.0 ); //10°/s pitch rate
    for(int ii=0; ii<1*50; ii++) //1s
    {
        ECF.Matrix_update();
        ECF.Normalize();
    }//for ii
    BOOST_CHECK(fabs(ECF.get_euler_angles_rad().toDeg().roll) < 1.0);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().pitch, (float)10.0, (float)1e-3);
    BOOST_CHECK(fabs(ECF.get_euler_angles_rad().toDeg().yaw) < 1.0);


    ECF.set_GyroVector_rads( 10.0*M_PI/180.0, 0.0, 0.0 ); //+ 10°/s roll rate
    for(int ii=0; ii<1*50; ii++) //1s
    {
        ECF.Matrix_update();
        ECF.Normalize();
    }//for ii
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().roll, (float)10.0, (float)1e-3);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().pitch, (float)10.0, (float)1e-3);
    BOOST_CHECK(fabs(ECF.get_euler_angles_rad().toDeg().yaw) < 1.0);

    //acceleration vector still initial, attitude should move to zero after a while...
    ECF.set_GyroVector_rads( 0.0, 0.0, 0.0 );
    // accel is: ECF.set_AccelVector(0.0, 0.0, 101);
    for(int ii=0; ii<60*50; ii++) // 60s
    {
        ECF.update();
    }//for ii
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll, (float)0.5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch, (float)0.5);
    //not corrected: BOOST_CHECK_SMALL(ECF.get_euler_angles().toDeg().yaw, (float)0.5);
}

BOOST_AUTO_TEST_CASE( Accel_adjust_test )
{

    ECFClass ECF;

    BOOST_CHECK_SMALL(ECF.Omega[0], (float)1e-6);
    BOOST_CHECK_SMALL(ECF.Omega[1], (float)1e-6);
    BOOST_CHECK_SMALL(ECF.Omega[2], (float)1e-6);

    ECF.f_ground_speed_ms  = 10.0;
    ECF.f_ground_course_deg = 0.0;
    ECF.f_speed_3d_ms      = 10.0;

    ECF.Omega[2]        = 2.0;  // omega_z = 2rad/s
    ECF.Accel_Vector[1] = 5.0;  // acc_y = 5m/s2

    ECF.Accel_adjust();
    BOOST_CHECK_CLOSE_FRACTION(ECF.Accel_Vector[1], (float)( 5.0+(10.0*2.0)), 1e-6 );
    BOOST_CHECK_CLOSE_FRACTION(ECF.Accel_Vector[2], (float)-9.81, (float)1e-6);

    ECF.f_ground_speed_ms = 8.0;
    ECF.f_speed_3d_ms     = 8.0;

    ECF.Omega[1]=-3.0;
    ECF.Omega[2]= 0.0;
    ECF.Accel_Vector[1] =  0.0;
    ECF.Accel_Vector[2] = -6.0;

    ECF.Accel_adjust();
    BOOST_CHECK_CLOSE_FRACTION(ECF.Accel_Vector[2], (float)( -6.0-(8.0*-3.0)), 1e-6 );
    BOOST_CHECK_SMALL(ECF.Accel_Vector[1], (float)1e-6);

}

BOOST_AUTO_TEST_CASE(ECF_set_parameter_test )
{
    ECFClass ECF;

    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_RollPitch(),  (float)-0.15,    (float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_RollPitch(),  (float)-0.00010, (float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_Yaw(),        (float)-4.0,     (float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_Yaw(),        (float)-4./1.e+6, (float)1e-6);

    ECF.set_Kp_RollPitch(-1.0);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_RollPitch(),  (float)-1.0, (float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_RollPitch(),  (float)-0.00010, (float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_Yaw(),        (float)-4.0,(float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_Yaw(),        (float)-4./1.e+6, (float)1e-6);

    ECF.set_Ki_RollPitch(-2.0);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_RollPitch(),  (float)-1.0, (float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_RollPitch(),  (float)-2.0, (float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_Yaw(),        (float)-4.0,(float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_Yaw(),        (float)-4./1.e+6, (float)1e-6);

    ECF.set_Kp_Yaw(-3.0);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_RollPitch(),  (float)-1.0, (float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_RollPitch(),  (float)-2.0, (float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_Yaw(),        (float)-3.0, (float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_Yaw(),        (float)-4./1.e+6, (float)1e-6);

    ECF.set_Ki_Yaw(-4.0);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_RollPitch(),  (float)-1.0, (float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_RollPitch(),  (float)-2.0, (float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_Yaw(),        (float)-3.0, (float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_Yaw(),        (float)-4.0, (float)1e-6);

    ECF.set_Ki_Yaw(5.0); // should not be set!
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_RollPitch(),  (float)-1.0, (float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_RollPitch(),  (float)-2.0, (float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_Yaw(),        (float)-3.0, (float)1e-6);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_Yaw(),        (float)-4.0, (float)1e-6);
}

BOOST_AUTO_TEST_CASE(ECF_tuning_test )
{
    ECFClass ECF;
    ECF.G_Dt = 0.02;

    //org. Kp = 1500*Ki
    ECF.set_Kp_RollPitch(-0.015);
    ECF.set_Ki_RollPitch(-0.00001);

//    //faster
//    ECF.set_Kp_RollPitch(-0.075);
//    ECF.set_Ki_RollPitch(-0.00005);

//    //slower
//    ECF.set_Kp_RollPitch(-0.0075);
//    ECF.set_Ki_RollPitch(-0.000005);


    ECF.set_AccelVector_mss( 6.9367, 0.0, -6.9367);
    //wait to settle attitude to 45deg
    for(int ii=0; ii<60*50; ii++) //60s
    {
        ECF.update();
    }
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_from_acc_rad().toDeg().pitch, (float)45.0, (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().roll , (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().yaw  , (float)1e-5);

 #include <iostream>
    ofstream outfile;
    outfile.open("history_pitch_acc.txt", ios_base::trunc);
    outfile<<"# Time\t"<<"pitch\t"<<"roll\t"<<"yaw"<<endl;
    for(int ii=0; ii<60*50; ii++) //60s
    {
        if(20*60 < ii)
           ECF.set_AccelVector_mss( 6.9367+3., 0.0, -6.9367); //2m/s2 more for a short periode of time
        if(22*60 < ii)
           ECF.set_AccelVector_mss( 6.9367, 0.0, -6.9367); //back to original acceleration

        ECF.update();
        outfile<<ii/50.<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().pitch<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().roll<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().yaw<<"\t"
        <<endl;
    }
    outfile.close();
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_from_acc_rad().toDeg().pitch, (float)45.0, (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().roll , (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().yaw  , (float)1e-5);

    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().pitch, (float)45.0, (float)0.5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll, (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw , (float)5.0);


    if (system(NULL))
    {
        /*int ret = */system ("gnuplot -persist plot_pitch_acc.gnuplot");
    }

}

BOOST_AUTO_TEST_CASE( Euler_from_ECF_with_mag )
{
    ECFClass ECF;
    ECF.G_Dt = 0.02;

    //check initialization
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll ,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw  ,  (float)1e-5);

    ECF.set_AccelVector_mss(0.0, 0.0, -9.81);
    ECF.set_MagVector_Gauss(0.4, 0.0, 0.4); //Pointing north + declination
    ECF.update();

    BOOST_CHECK_SMALL(ECF.get_Acceleration_mss(0),  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_Acceleration_mss(1),  (float)1e-5);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Acceleration_mss(2), (float)-9.81, (float)1e-5);

    BOOST_CHECK_CLOSE_FRACTION(ECF.errorRollPitch[0], 0.0, 1e-5);
    BOOST_CHECK_CLOSE_FRACTION(ECF.errorRollPitch[1], 0.0, 1e-5);
    BOOST_CHECK_CLOSE_FRACTION(ECF.errorRollPitch[2], 0.0, 1e-5);

    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().pitch,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().roll ,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().yaw  ,  (float)1e-5);

    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll ,  (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw  ,  (float)1e-5);

    ECF.set_AccelVector_mss( 6.9367, 0.0, -6.9367);
    ECF.set_MagVector_Gauss(0.565, 0.0, 0.0); //Pointing north + declination

#include <iostream>
    ofstream outfile;
    outfile.open("history_pitch_mag.txt", ios_base::trunc);
    outfile<<"# Time\t"<<"pitch\t"<<"roll\t"<<"yaw"<<endl;
    for(int ii=0; ii<60*50; ii++) //60s
    {
        ECF.update();
        outfile<<ii/50.<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().pitch<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().roll<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().yaw<<"\t"
        <<endl;
    }
    outfile.close();
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_from_acc_rad().toDeg().pitch, (float)45.0, (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().roll , (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().yaw  , (float)1e-5);

    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().pitch, (float)45.0, (float)0.5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll, (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw , (float)1e-5);

    ECF.set_AccelVector_mss(0.0, -6.9367, -6.9367);
    ECF.set_MagVector_Gauss(0.4, 0.283, 0.283); //Pointing north + declination

    outfile.open("history_roll_mag.txt", ios_base::trunc);
    outfile<<"# Time\t"<<"pitch\t"<<"roll\t"<<"yaw"<<endl;
    for(int ii=0; ii<60*50; ii++) //60s
    {
        ECF.update();
        outfile<<ii/50.<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().pitch<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().roll<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().yaw<<"\t"
        <<endl;
    }
    outfile.close();

    //ECF is at pitch=45° and roll=45°
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().pitch,  (float)1e-5);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_from_acc_rad().toDeg().roll , (float)45.0, (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().yaw  ,  (float)1e-5);

    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch, (float)0.5);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().roll , (float)45.0, (float)0.5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw, (float)0.5);
}

BOOST_AUTO_TEST_CASE(ECF_tuning_test_with_mag )
{
    ECFClass ECF;
    ECF.G_Dt = 0.02;

    //org. Kp = 1500*Ki
    ECF.set_Kp_RollPitch(-0.015);
    ECF.set_Ki_RollPitch(-0.00001);

//    //faster
//    ECF.set_Kp_RollPitch(-0.075);
//    ECF.set_Ki_RollPitch(-0.00005);

//    //slower
//    ECF.set_Kp_RollPitch(-0.0075);
//    ECF.set_Ki_RollPitch(-0.000005);


    ECF.set_AccelVector_mss( 6.9367, 0.0, -6.9367);
    ECF.set_MagVector_Gauss(0.4, 0.0, 0.4); //Pointing north + declination

    //wait to settle attitude to 45deg
    for(int ii=0; ii<60*50; ii++) //60s
    {
        ECF.update();
    }
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_from_acc_rad().toDeg().pitch, (float)45.0, (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().roll , (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().yaw  , (float)1e-5);

 #include <iostream>
    ofstream outfile;
    outfile.open("history_pitch_acc_mag.txt", ios_base::trunc);
    outfile<<"# Time\t"<<"pitch\t"<<"roll\t"<<"yaw"<<endl;
    for(int ii=0; ii<60*50; ii++) //60s
    {
        if(20*60 < ii)
           ECF.set_AccelVector_mss( 6.9367+3., 0.0, -6.9367); //2m/s2 more for a short periode of time
        if(22*60 < ii)
           ECF.set_AccelVector_mss( 6.9367, 0.0, -6.9367); //back to original acceleration

        ECF.update();
        outfile<<ii/50.<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().pitch<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().roll<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().yaw<<"\t"
        <<endl;
    }
    outfile.close();
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_from_acc_rad().toDeg().pitch, (float)45.0, (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().roll , (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().yaw  , (float)1e-5);

    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().pitch, (float)45.0, (float)0.5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll, (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw , (float)1e-5);


    if (system(NULL))
    {
        /*int ret = */system ("gnuplot -persist plot_result_mag.gnuplot");
    }

}

BOOST_AUTO_TEST_CASE(ECF_heading_update_with_mag )
{
    ECFClass ECF;
    ECF.G_Dt = 0.02;
//    ECF.set_Kp_Yaw(-4.);
//    ECF.set_Ki_Yaw(-4./1.e+6);

    ECF.set_AccelVector_mss(0, 0.0, -9.81);
    ECF.set_MagVector_Gauss(0.2, 0.0, 0.6); //Pointing north + declination

    ECF.update();

    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().pitch, (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().roll , (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().yaw  , (float)1e-5);

 #include <iostream>
    ofstream outfile;
    outfile.open("history_yaw_mag.txt", ios_base::trunc);
    outfile<<"# Time\t"<<"roll\t"<<"pitch\t"<<"yaw\t"<< "f_heading\t"<<"errorCourse\t"<<endl;
    for(int ii=0; ii<60*50; ii++) //60s
    {
        ECF.set_MagVector_Gauss(0.1, 0.1, 0.6); //-45deg

        ECF.update();
        outfile<<ii/50.<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().roll<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().pitch<<"\t"
        <<ECF.get_euler_angles_rad().toDeg().yaw<<"\t"
        <<endl;
    }
    outfile.close();
    BOOST_CHECK_SMALL(ECF.errorYaw[0], (float)1e-5);
    BOOST_CHECK_SMALL(ECF.errorYaw[1], (float)1e-5);
    BOOST_CHECK_SMALL(ECF.errorYaw[2], (float)1e-3);


    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().pitch, (float)1e-5);
    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().roll , (float)1e-5);
    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().yaw  , (float)-45.0, (float)1e-3);

    if (system(NULL))
    {
        int ret = system ("gnuplot -persist plot_result_yaw_mag.gnuplot");
        (void)ret;
    }
}


BOOST_AUTO_TEST_SUITE_END( )
