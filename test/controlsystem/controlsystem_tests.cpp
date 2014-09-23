#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>


using namespace std;

#include "../app/brainware/ecf_module.h"
#include "../app/brainware/ctrl_module.h"
void test_controlsystem(void)
{
    ecf_module m_ecf;
    float vel_north = 0.f, vel_east = 0.f, vel_down = 0.f;
    m_ecf.get_sf_ptr()->set_speed_ms(vel_north, vel_east, vel_down);

    uint16_t ped_us, col_us, lon_us, lat_us;
    ctrl_module m_ctrl(m_ecf.get_sf_ptr());
    m_ctrl.getControl(&ped_us, &col_us, &lon_us, &lat_us);
    BOOST_CHECK_EQUAL(ped_us, 1500);
    BOOST_CHECK_EQUAL(col_us, 1500);
    BOOST_CHECK_EQUAL(lon_us, 1500);
    BOOST_CHECK_EQUAL(lat_us, 1500);

    sleep(1);
}
