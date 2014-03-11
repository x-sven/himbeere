#include <iostream>

#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "c_flightgear_sender_asio.hpp"


namespace pt = boost::posix_time;


 int main(int argc, char* argv[])
 {

    double roll = 0;

    c_flightgear_sender_asio FGM("129.247.48.80", 5507);

    while(1)
    {

        FGM.set_position_wgs84( 53.123*3.14/180., 10.456*3.14/180., 100.);
        FGM.set_attiude_euler((float)(++roll*3.14/180.), 0., 0.);

        FGM.update();

        boost::this_thread::sleep(pt::milliseconds(50)); //around 10Hz
    }
    return(0);
}
