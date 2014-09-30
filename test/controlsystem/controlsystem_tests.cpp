#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>


using namespace std;

#include "Delay.h"
#include "../app/brainware/ecf_module.h"
#include "../app/brainware/ctrl_module.h"

void test_controlsystem(void)
{
    {   // default initialization
        float vel_north = 0.f, vel_east = 0.f, vel_down = 0.f;
        uint16_t ped_us = 0, col_us = 0, lon_us = 0, lat_us = 0;
        ecf_module m_ecf;
        m_ecf.get_sf_ptr()->set_speed_ms(vel_north, vel_east, vel_down);
        ctrl_module m_ctrl(m_ecf.get_sf_ptr());
        m_ctrl.getControl(&ped_us, &col_us, &lon_us, &lat_us);
        BOOST_CHECK_EQUAL(ped_us, 1500);
        BOOST_CHECK_EQUAL(col_us, 1500);
        BOOST_CHECK_EQUAL(lon_us, 1500);
        BOOST_CHECK_EQUAL(lat_us, 1500);
    }
    {   // default command forward
        float vel_north = -5.f, vel_east = 0.f, vel_down = 0.f;
        uint16_t ped_us = 0, col_us = 0, lon_us = 0, lat_us = 0;
        ecf_module m_ecf;
        m_ecf.get_sf_ptr()->set_speed_ms(vel_north, vel_east, vel_down);
        ctrl_module m_ctrl(m_ecf.get_sf_ptr());
        m_ctrl.getControl(&ped_us, &col_us, &lon_us, &lat_us);
        BOOST_CHECK_EQUAL(ped_us, 1500);
        BOOST_CHECK_EQUAL(col_us, 1500);
        BOOST_CHECK_EQUAL(lon_us, 714);
        BOOST_CHECK_EQUAL(lat_us, 1500);
    }
    {   // default command backwards
        float vel_north = +5.f, vel_east = 0.f, vel_down = 0.f;
        uint16_t ped_us = 0, col_us = 0, lon_us = 0, lat_us = 0;
        ecf_module m_ecf;
        m_ecf.get_sf_ptr()->set_speed_ms(vel_north, vel_east, vel_down);
        ctrl_module m_ctrl(m_ecf.get_sf_ptr());
        m_ctrl.getControl(&ped_us, &col_us, &lon_us, &lat_us);
        BOOST_CHECK_EQUAL(ped_us, 1500);
        BOOST_CHECK_EQUAL(col_us, 1500);
        BOOST_CHECK_EQUAL(lon_us, 2285);
        BOOST_CHECK_EQUAL(lat_us, 1500);
    }
    {   // default command left
        float vel_north = 0.f, vel_east = +5.f, vel_down = 0.f;
        uint16_t ped_us = 0, col_us = 0, lon_us = 0, lat_us = 0;
        ecf_module m_ecf;
        m_ecf.get_sf_ptr()->set_speed_ms(vel_north, vel_east, vel_down);
        ctrl_module m_ctrl(m_ecf.get_sf_ptr());
        m_ctrl.getControl(&ped_us, &col_us, &lon_us, &lat_us);
        BOOST_CHECK_EQUAL(ped_us, 1500);
        BOOST_CHECK_EQUAL(col_us, 1500);
        BOOST_CHECK_EQUAL(lon_us, 1500);
        BOOST_CHECK_EQUAL(lat_us, 2285);
    }
    {   // default command right
        float vel_north = 0.f, vel_east = -5.f, vel_down = 0.f;
        uint16_t ped_us = 0, col_us = 0, lon_us = 0, lat_us = 0;
        ecf_module m_ecf;
        m_ecf.get_sf_ptr()->set_speed_ms(vel_north, vel_east, vel_down);
        ctrl_module m_ctrl(m_ecf.get_sf_ptr());
        m_ctrl.getControl(&ped_us, &col_us, &lon_us, &lat_us);
        BOOST_CHECK_EQUAL(ped_us, 1500);
        BOOST_CHECK_EQUAL(col_us, 1500);
        BOOST_CHECK_EQUAL(lon_us, 1500);
        BOOST_CHECK_EQUAL(lat_us, 714);
    }
    {   // default command up
        float vel_north = 0.f, vel_east = 0.f, vel_down = +5.f;
        uint16_t ped_us = 0, col_us = 0, lon_us = 0, lat_us = 0;
        ecf_module m_ecf;
        m_ecf.get_sf_ptr()->set_speed_ms(vel_north, vel_east, vel_down);
        ctrl_module m_ctrl(m_ecf.get_sf_ptr());
        m_ctrl.getControl(&ped_us, &col_us, &lon_us, &lat_us);
        BOOST_CHECK_EQUAL(ped_us, 1500);
        BOOST_CHECK_EQUAL(col_us, 2285);
        BOOST_CHECK_EQUAL(lon_us, 1500);
        BOOST_CHECK_EQUAL(lat_us, 1500);
    }
    {   // default command down
        float vel_north = 0.f, vel_east = 0.f, vel_down = -5.f;
        uint16_t ped_us = 0, col_us = 0, lon_us = 0, lat_us = 0;
        ecf_module m_ecf;
        m_ecf.get_sf_ptr()->set_speed_ms(vel_north, vel_east, vel_down);
        ctrl_module m_ctrl(m_ecf.get_sf_ptr());
        m_ctrl.getControl(&ped_us, &col_us, &lon_us, &lat_us);
        BOOST_CHECK_EQUAL(ped_us, 1500);
        BOOST_CHECK_EQUAL(col_us, 714);
        BOOST_CHECK_EQUAL(lon_us, 1500);
        BOOST_CHECK_EQUAL(lat_us, 1500);
    }
    {   // 45 deg Heading, compensation to North
        float vel_north = -5.f, vel_east = 0.f, vel_down = 0.f;
        uint16_t ped_us = 0, col_us = 0, lon_us = 0, lat_us = 0;

        ECFClass ECF;
        ECF.G_Dt = 0.02;
        //check initialization
        BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch,  (float)1e-5);
        BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll ,  (float)1e-5);
        BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().yaw  ,  (float)1e-5);
        ECF.set_AccelVector_mss(0.0, 0.0, -9.81);
        ECF.update();
        BOOST_CHECK_SMALL(ECF.get_Acceleration_mss(0),  (float)1e-5);
        BOOST_CHECK_SMALL(ECF.get_Acceleration_mss(1),  (float)1e-5);
        BOOST_CHECK_CLOSE_FRACTION(ECF.get_Acceleration_mss(2), (float)-9.81, (float)1e-5);
        ECF.set_GyroVector_rads(0., 0., 10*M_PI/180.);
        for(int ii=0; ii<50*4.5; ii++) //4.5s * 10deg/s = 45deg
        {
            ECF.update();
        }
        BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().pitch, (float)1e-5);
        BOOST_CHECK_SMALL(ECF.get_euler_angles_rad().toDeg().roll , (float)1e-5);
        BOOST_CHECK_CLOSE_FRACTION(ECF.get_euler_angles_rad().toDeg().yaw, (float)45.0, (float)1e-5);
        // calculate control output
        ECF.set_speed_ms(vel_north, vel_east, vel_down);
        SensorFusion  *m_sf_ptr = static_cast<SensorFusion*>(&ECF);
        ctrl_module m_ctrl(m_sf_ptr);
        m_ctrl.getControl(&ped_us, &col_us, &lon_us, &lat_us);
        BOOST_CHECK_EQUAL(ped_us, 1500);
        BOOST_CHECK_EQUAL(col_us, 1500);
        BOOST_CHECK_EQUAL(lon_us, 714); // forward
        BOOST_CHECK_EQUAL(lat_us, 2285);// to the left
    }
}
