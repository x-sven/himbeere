#include <iostream>
#include <signal.h>

#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/timer/timer.hpp> //http://www.boost.org/doc/libs/1_52_0/libs/timer/doc/cpu_timers.html


#include "c_joystick.hpp"

#include "c_joystick_manager.hpp"

using namespace std;
namespace po = boost::program_options;
namespace pt = boost::posix_time;

bool execute = true;

cJoystick* Joy;

cJoystickManager JoyManager;


//***************************************
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

//    Joy = new cJoystick;
//
//    cout << " Name of the Joystick: "   << Joy->getName()
//         << ", Buttons: "               << Joy->getNumberOfButtons()
//         << ", Axis: "                  << Joy->getNumberOfAxes()
//         << endl;
//
//
//    while(execute)
//    {
//
//        for(int ii=0; ii < Joy->getNumberOfAxes(); ii++)
//        {
//            cout << " A(" << ii << "): " << Joy->getAxis(ii);
//
//        }
//        cout << endl;
//
//        boost::this_thread::sleep(pt::milliseconds(100)); //arround 10 Hz
//    }

    return(0);
}

