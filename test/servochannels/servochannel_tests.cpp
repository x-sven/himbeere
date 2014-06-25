#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>

#include "c_servochannels.h"

using namespace std;


void test_servochannels_constructor(void)
{

    cServoChannels channel_raspi_6(6);
    cServoChannels channel_dummy_4(4);

    BOOST_CHECK(true != channel_raspi_6.valid());
    BOOST_CHECK(true != channel_dummy_4.valid());
    BOOST_CHECK(6 == channel_raspi_6.size());
    BOOST_CHECK(4 == channel_dummy_4.size());

    for(size_t ii=0; ii < channel_dummy_4.size(); ii++)
        channel_dummy_4[ii]=ii;

    // copy
    channel_raspi_6 = channel_dummy_4;
    BOOST_CHECK_EQUAL(channel_raspi_6.size(), 4); // is now only 4!!!
    BOOST_CHECK_EQUAL(channel_dummy_4.size(), 4);

    // valid
    for(size_t ii=0; ii < channel_raspi_6.size(); ii++)
        BOOST_CHECK(channel_raspi_6[ii]==ii);

    BOOST_CHECK(true != channel_raspi_6.valid());

    // brackets
    for(size_t ii=0; ii < channel_raspi_6.size(); ii++)
        channel_raspi_6[ii] = 1500+ii;

    BOOST_CHECK(true == channel_raspi_6.valid());
    BOOST_CHECK(4 == channel_raspi_6.size());
    BOOST_CHECK(true != channel_dummy_4.valid()); // must not valid
    BOOST_CHECK(4 == channel_dummy_4.size());

    channel_dummy_4 = channel_raspi_6;

    BOOST_CHECK(true == channel_raspi_6.valid());
    BOOST_CHECK(4 == channel_raspi_6.size());
    BOOST_CHECK(true == channel_dummy_4.valid());
    BOOST_CHECK(4 == channel_dummy_4.size());

    BOOST_CHECK(channel_dummy_4[rud_1] == 1500);
    BOOST_CHECK(channel_dummy_4[thr_2] == 1501);
    BOOST_CHECK(channel_dummy_4[ele_3] == 1502);
    BOOST_CHECK(channel_dummy_4[ail_4] == 1503);
}
