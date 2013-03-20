#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <cmath>
#include <ostream>
#include <inttypes.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/thread.hpp>

//!< First read this: http://www.boost.org/doc/libs/1_53_0/libs/test/doc/html/utf/user-guide.html

//!< The following define is used to access privat methods
//!< without modifications of the included classes.
#define private public

#include "Drotek10dof.h"

using namespace std;

Drotek10dof imu10dof;
int cb_counter = 0;
boost::posix_time::ptime start_time;
bool thread_running = false;

void imu_cb_counter(void)
{
    boost::posix_time::ptime cb_time = (boost::posix_time::microsec_clock::local_time());
    boost::posix_time::time_duration interval_imu(boost::posix_time::microseconds(1000000 / 200)); //100 Hz
    boost::posix_time::time_duration time_error = start_time - cb_time + interval_imu;

    if(cb_counter > 10) // skip first 10 calls
    {
        BOOST_CHECK_SMALL((double)time_error.total_microseconds() , 1000.);
    }
//    cout<< boost::posix_time::to_simple_string(cb_time)
//        << " == "
//        << boost::posix_time::to_simple_string(start_time+interval_imu)
//        << " ?" << endl;
    cb_counter++;
    start_time = (boost::posix_time::microsec_clock::local_time());
}

void while_function(void)
{
    volatile int ii = 0;
    while(thread_running)
    {
        //waste time;
        ii++;
        boost::this_thread::sleep(boost::posix_time::microsec(10));
        ii--;
    }

}


BOOST_AUTO_TEST_CASE( call_back_timing )
{
    //register function
    imu10dof.connect_imu_callback( (boost::function<void (void)>)imu_cb_counter );

    start_time = (boost::posix_time::microsec_clock::local_time());

    //start thread
    imu10dof.begin(200,10,SCHED_FIFO);

    //wait
    boost::this_thread::sleep(boost::posix_time::milliseconds(1001));

    //check
    BOOST_CHECK_EQUAL(cb_counter, 200); //1.001 sec -> 100 loop calls!?

    //std::cout << "*****Starting Thread*****" << std::endl;

    thread_running = true;
    boost::thread time_wasting_thread(&while_function);

    //std::cout << "********Waiting********" << std::endl;

    //wait
    boost::this_thread::sleep(boost::posix_time::milliseconds(1001));
    thread_running = false;

    //std::cout << "********Terminated********" << std::endl;

    BOOST_CHECK_EQUAL(cb_counter, 400); //1.001 sec -> 100 loop calls!?

    //terminate
    imu10dof.end();
}





//BOOST_AUTO_TEST_CASE( Euler_from_ECF )
//{
//    ECFClass ECF;
//
//    //check initialization
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch,  (float)1e-5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll ,  (float)1e-5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw  ,  (float)1e-5);
//
//    ECF.set_AccelVector_mss(0.0, 0.0, 9.81);
//    ECF.update();
//
//    BOOST_CHECK_SMALL(ECF.get_Acceleration_mss(0),  (float)1e-5);
//    BOOST_CHECK_SMALL(ECF.get_Acceleration_mss(1),  (float)1e-5);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Acceleration_mss(2), (float)9.81, (float)1e-5);
//
//    BOOST_CHECK_CLOSE_FRACTION(ECF.errorRollPitch[0], 0.0, 1e-5);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.errorRollPitch[1], 0.0, 1e-5);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.errorRollPitch[2], 0.0, 1e-5);
//
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().pitch,  (float)1e-5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().roll ,  (float)1e-5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().yaw  ,  (float)1e-5);
//
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch,  (float)1e-5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll ,  (float)1e-5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw  ,  (float)1e-5);
//
//    ECF.set_AccelVector_mss(-6.9367, 0.0, 6.9367);
//
//#include <iostream>
//    ofstream outfile;
//    outfile.open("history_pitch.txt", ios_base::trunc);
//    outfile<<"# Time\t"<<"pitch\t"<<"roll\t"<<"yaw"<<endl;
//    for(int ii=0; ii<60*50; ii++)
//    {
//        ECF.update();
//        outfile<<ii/50.<<"\t"
//        <<ECF.get_euler_angles_rad().toDeg().pitch<<"\t"
//        <<ECF.get_euler_angles_rad().toDeg().roll<<"\t"
//        <<ECF.get_euler_angles_rad().toDeg().yaw<<"\t"
//        <<endl;
//    }
//    outfile.close();
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_from_acc_rad().toDeg().pitch, (float)45.0, (float)1e-5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().roll , (float)1e-5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().yaw  , (float)1e-5);
//
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().pitch, (float)45.0, (float)0.5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll, (float)1e-5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw , (float)5.0);
//
//    ECF.set_AccelVector_mss(0.0, 6.9367, 6.9367);
//
//    outfile.open("history_roll.txt", ios_base::trunc);
//    outfile<<"# Time\t"<<"pitch\t"<<"roll\t"<<"yaw"<<endl;
//    for(int ii=0; ii<60*50; ii++)
//    {
//        ECF.update();
//        outfile<<ii/50.<<"\t"
//        <<ECF.get_euler_angles_rad().toDeg().pitch<<"\t"
//        <<ECF.get_euler_angles_rad().toDeg().roll<<"\t"
//        <<ECF.get_euler_angles_rad().toDeg().yaw<<"\t"
//        <<endl;
//    }
//    outfile.close();
//
//    //ECF is at pitch=45° --> reduce pitch increase roll...
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().pitch,  (float)1e-5);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_from_acc_rad().toDeg().roll , (float)45.0, (float)1e-5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().yaw  ,  (float)1e-5);
//
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch, (float)0.5);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().roll , (float)45.0, (float)0.5);
//    //BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw, (float)10.0); //for some reason it drifts a little bit
//}
//
//BOOST_AUTO_TEST_CASE( Matrix_update_test )
//{
//
//    ECFClass ECF;
//
//    BOOST_CHECK_CLOSE_FRACTION(ECF.G_Dt, (float)0.02, (float)1e-6);
//
//    ECF.set_GyroVector_rads( 0.0, 0.0, 0.0 );
//    ECF.Matrix_update();
//    ECF.Normalize();
//
//    for (uint8_t ii=0; ii<3; ii++)
//    {
//        BOOST_CHECK_SMALL(ECF.get_GyroRate_rads(ii),  (float)1e-5);
//        BOOST_CHECK_SMALL(ECF.get_CorrectedRate_rads(ii),  (float)1e-5);
//        for (uint8_t jj=0; jj<3; jj++)
//        {
//            if(ii==jj)
//                BOOST_CHECK_CLOSE_FRACTION(ECF.ECF_Matrix[ii][jj], (float)1.0, (float)1e-5);
//            else
//                BOOST_CHECK_SMALL(ECF.ECF_Matrix[ii][jj], (float)1e-5);
//        }//for jj
//    }//for ii
//
//    ECF.set_GyroVector_rads( 0.0, 0.0, 10.0*M_PI/180.0 ); //10°/s yaw rate
//    for(int ii=0; ii<1*5; ii++)
//    {
//        ECF.Matrix_update();
//        ECF.Normalize();
//    }//for ii
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll,  (float)1e-6);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch, (float)1e-6);
//    BOOST_CHECK(fabs(ECF.get_euler_angles_rad().toDeg().yaw) < 10.0);
//
//    ECF.set_GyroVector_rads( 0.0, 10.0*M_PI/180.0, 0.0 ); //10°/s pitch rate
//    for(int ii=0; ii<1*50; ii++) //1s
//    {
//        ECF.Matrix_update();
//        ECF.Normalize();
//    }//for ii
//    BOOST_CHECK(fabs(ECF.get_euler_angles_rad().toDeg().roll) < 1.0);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().pitch, (float)10.0, (float)1e-3);
//    BOOST_CHECK(fabs(ECF.get_euler_angles_rad().toDeg().yaw) < 1.0);
//
//
//    ECF.set_GyroVector_rads( 10.0*M_PI/180.0, 0.0, 0.0 ); //+ 10°/s roll rate
//    for(int ii=0; ii<1*50; ii++) //1s
//    {
//        ECF.Matrix_update();
//        ECF.Normalize();
//    }//for ii
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().roll, (float)10.0, (float)1e-3);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().pitch, (float)10.0, (float)1e-3);
//    BOOST_CHECK(fabs(ECF.get_euler_angles_rad().toDeg().yaw) < 1.0);
//
//    //acceleration vector still initial, attitude should move to zero after a while...
//    ECF.set_GyroVector_rads( 0.0, 0.0, 0.0 );
//    // accel is: ECF.set_AccelVector(0.0, 0.0, 101);
//    for(int ii=0; ii<60*50; ii++)
//    {
//        ECF.update();
//    }//for ii
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll, (float)0.5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch, (float)0.5);
//    //not corrected: BOOST_CHECK_SMALL(ECF.get_euler_angles().toDeg().yaw, (float)0.5);
//}
//
//BOOST_AUTO_TEST_CASE( Accel_adjust_test )
//{
//
//    ECFClass ECF;
//
//    BOOST_CHECK_SMALL(ECF.Omega[0], (float)1e-6);
//    BOOST_CHECK_SMALL(ECF.Omega[1], (float)1e-6);
//    BOOST_CHECK_SMALL(ECF.Omega[2], (float)1e-6);
//
//    ECF.f_speed_3d = 10.0;
//    ECF.Omega[2]=2.0;
//    ECF.Accel_Vector[1] = 5.0;
//
//    ECF.Accel_adjust();
//    BOOST_CHECK_CLOSE_FRACTION(ECF.Accel_Vector[1], (float)( 5.0+(10.0*2.0)), 1e-6 );
//    BOOST_CHECK_CLOSE_FRACTION(ECF.Accel_Vector[2], (float)9.81, (float)1e-6);
//
//    ECF.f_speed_3d = 8.0;
//    ECF.Omega[1]=-3.0;
//    ECF.Omega[2]= 0.0;
//    ECF.Accel_Vector[1] =  0.0;
//    ECF.Accel_Vector[2] = -6.0;
//
//    ECF.Accel_adjust();
//    BOOST_CHECK_CLOSE_FRACTION(ECF.Accel_Vector[2], (float)( -6.0-(8.0*-3.0)), 1e-6 );
//    BOOST_CHECK_SMALL(ECF.Accel_Vector[1], (float)1e-6);
//
//}
//
//BOOST_AUTO_TEST_CASE(ECF_parameter_test )
//{
//    ECFClass ECF;
//
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_RollPitch(),  (float)0.15,    (float)1e-6);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_RollPitch(),  (float)0.00010, (float)1e-6);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_Yaw(),        (float)0.5,     (float)1e-6);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_Yaw(),        (float)0.00005, (float)1e-6);
//
//    ECF.set_Kp_RollPitch(1.0);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_RollPitch(),  (float)1.0, (float)1e-6);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_RollPitch(),  (float)0.00010, (float)1e-6);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_Yaw(),        (float)0.5,     (float)1e-6);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_Yaw(),        (float)0.00005, (float)1e-6);
//
//    ECF.set_Ki_RollPitch(2.0);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_RollPitch(),  (float)1.0, (float)1e-6);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_RollPitch(),  (float)2.0, (float)1e-6);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_Yaw(),        (float)0.5,     (float)1e-6);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_Yaw(),        (float)0.00005, (float)1e-6);
//
//    ECF.set_Kp_Yaw(3.0);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_RollPitch(),  (float)1.0, (float)1e-6);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_RollPitch(),  (float)2.0, (float)1e-6);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_Yaw(),        (float)3.0,     (float)1e-6);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_Yaw(),        (float)0.00005, (float)1e-6);
//
//    ECF.set_Ki_Yaw(4.0);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_RollPitch(),  (float)1.0, (float)1e-6);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_RollPitch(),  (float)2.0, (float)1e-6);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Kp_Yaw(),        (float)3.0,     (float)1e-6);
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_Ki_Yaw(),        (float)4.0, (float)1e-6);
//}
//
//BOOST_AUTO_TEST_CASE(ECF_tuning_test )
//{
//    ECFClass ECF;
//
//    //org. Kp = 1500*Ki
//    ECF.set_Kp_RollPitch(0.015);
//    ECF.set_Ki_RollPitch(0.00001);
//
//    //faster
//    ECF.set_Kp_RollPitch(0.075);
//    ECF.set_Ki_RollPitch(0.00005);
//
////    //slower
////    ECF.set_Kp_RollPitch(0.0075);
////    ECF.set_Ki_RollPitch(0.000005);
//
//
//    ECF.set_AccelVector_mss(-6.9367, 0.0, 6.9367);
//    //wait to settle attitude to 45deg
//    for(int ii=0; ii<60*50; ii++)
//    {
//        ECF.update();
//    }
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_from_acc_rad().toDeg().pitch, (float)45.0, (float)1e-5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().roll , (float)1e-5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().yaw  , (float)1e-5);
//
// #include <iostream>
//    ofstream outfile;
//    outfile.open("history_pitch_acc.txt", ios_base::trunc);
//    outfile<<"# Time\t"<<"pitch\t"<<"roll\t"<<"yaw"<<endl;
//    for(int ii=0; ii<60*50; ii++)
//    {
//        if(20*60 < ii)
//           ECF.set_AccelVector_mss(-6.9367+3., 0.0, 6.9367); //2m/s2 more for a short periode of time
//        if(22*60 < ii)
//           ECF.set_AccelVector_mss(-6.9367, 0.0, 6.9367); //back to original acceleration
//
//        ECF.update();
//        outfile<<ii/50.<<"\t"
//        <<ECF.get_euler_angles_rad().toDeg().pitch<<"\t"
//        <<ECF.get_euler_angles_rad().toDeg().roll<<"\t"
//        <<ECF.get_euler_angles_rad().toDeg().yaw<<"\t"
//        <<endl;
//    }
//    outfile.close();
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_from_acc_rad().toDeg().pitch, (float)45.0, (float)1e-5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().roll , (float)1e-5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_from_acc_rad().toDeg().yaw  , (float)1e-5);
//
//    BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().pitch, (float)45.0, (float)0.5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll, (float)1e-5);
//    BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw , (float)5.0);
//
//
//    if (system(NULL))
//    {
//        /*int ret = */system ("gnuplot -persist plot_roll.gnuplot");
//    }
//
//}
