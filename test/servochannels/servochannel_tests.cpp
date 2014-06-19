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

    cServoChannels channel_raspi(4);

    // copy
    channel_raspi = parser.get_channels();

    // valid
    channel_raspi.valid();

    // brackets
    channel_raspi[rud_1];

}
