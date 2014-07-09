#include <iostream>
#include <signal.h>

#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/timer/timer.hpp> //http://www.boost.org/doc/libs/1_52_0/libs/timer/doc/cpu_timers.html

#include "c_joystick_manager.hpp"

#include "c_flightgear_sender_asio.hpp"
c_flightgear_sender_asio FGM("192.168.1.60", 5507);

#include "c_simulation.h"
c_simulation sim;

using namespace std;
namespace po = boost::program_options;
namespace pt = boost::posix_time;

bool execute = true;

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

    sim.begin();
    sim.get_sample_time();

    cJoystickManager joystick;

    //axis, scale, upper limit, offset, lower limit, fail
    joystick.setRollProberty    (0, 500, 2100, 1500, 900, 1500);
    joystick.setPitchProberty   (5, 500, 2100, 1500, 900, 1500);
    joystick.setYawProberty     (2, 500, 2100, 1500, 900, 1500);
    joystick.setColProberty     (1, 500, 2100, 1500, 900, 1500);
    joystick.setModeProberty    (4, 500, 2100, 1500, 900, 1500);

    uint8_t rate_divider = 0;

    while(execute)
    {

//        printf("roll: % 4.1f pitch: % 4.1f yaw: % 4.1f col: % 4.1f mode: % 4.1f\n",
//               joystick.getRollValue(),
//               joystick.getPitchValue(),
//               joystick.getYawValue(),
//               joystick.getColValue(),
//               joystick.getModeValue()
//              );

        sim.set_input(joystick.getRollValue(),
                      joystick.getPitchValue(),
                      joystick.getYawValue(),
                      joystick.getColValue());
        sim.update();

        if(rate_divider > 4) // 20 Hz
        {
            FGM.set_attiude_quaternion(
                sim.get_output()->motion.attitude.q0,
                sim.get_output()->motion.attitude.q1,
                sim.get_output()->motion.attitude.q2,
                sim.get_output()->motion.attitude.q3);

            FGM.set_position_ned(
                sim.get_output()->motion.pos.x,
                sim.get_output()->motion.pos.y,
                sim.get_output()->motion.pos.z,
                52.319602*3.14/180.,            // BS runway
                10.563912*3.14/180.,
                100.);
            FGM.update();

//            cout << "x: "   << sim.get_output()->motion.pos.x
//                 << ", y: " << sim.get_output()->motion.pos.y
//                 << ", z: " << sim.get_output()->motion.pos.z
//                 << std::endl;

            rate_divider = 0;
        }
        else rate_divider++;

        boost::this_thread::sleep( pt::microseconds(1e+6*sim.get_sample_time()) );
        //boost::this_thread::sleep(pt::milliseconds(100)); //around 10 Hz
    }

    return(0);
}

