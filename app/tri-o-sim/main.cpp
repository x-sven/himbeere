#include <iostream>
#include <signal.h>

#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/timer/timer.hpp> //http://www.boost.org/doc/libs/1_52_0/libs/timer/doc/cpu_timers.html

#include "c_joystick.hpp"
//#include "c_joystick_manager.hpp"

#include "c_flightgear_sender_asio.hpp"
c_flightgear_sender_asio FGM("192.168.1.57", 5507);

#include "src/c_simulation.h"
c_simulation sim;

using namespace std;
namespace po = boost::program_options;
namespace pt = boost::posix_time;

bool execute = true;

cJoystick* Joy;
//cJoystickManager JoyManager;

/***************************************/
void trap(int signal)
{
    cout << "\n SIGINT received!" << endl;
    execute = false;
}



int main(int argc, char **argv )
{
    boost::timer::auto_cpu_timer t(1,"Program has within the %w s run time the CPU used for %t s (%p \%).\n\n");

    // register Ctrl-C signal
    signal(SIGINT, &trap);

    std::cout << "Hello world!" << std::endl;

    Joy = new cJoystick;

    cout << " Name of the Joystick: "   << "(disabled)" //Joy->getName()
         << ", Buttons: "               << Joy->getNumberOfButtons()
         << ", Axis: "                  << Joy->getNumberOfAxes()
         << endl;

    sim.begin();
    sim.get_sample_time();

    while(execute)
    {

        for(int ii=0; ii < Joy->getNumberOfAxes(); ii++)
        {
            cout << " A(" << ii << "): " << Joy->getAxis(ii);

        }
        cout << endl;


        sim.set_input(1500, 1500, 1500, 1000);

        sim.update();

//        cout << "x: "   << sim.get_output()->motion.pos.x
//             << ", y: " << sim.get_output()->motion.pos.y
//             << ", z: " << sim.get_output()->motion.pos.z
//             << std::endl;

        //boost::this_thread::sleep( pt::microseconds(1e+6*sim.get_sample_time()) );
        boost::this_thread::sleep(pt::milliseconds(100)); //around 10 Hz
    }



    delete(Joy);
    return(0);
}

