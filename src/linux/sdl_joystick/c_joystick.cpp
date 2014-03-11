#include <iostream>
#include <vector>
#include <boost/timer/timer.hpp> //http://www.boost.org/doc/libs/1_52_0/libs/timer/doc/cpu_timers.html


#include "c_joystick.hpp"

using namespace std;
namespace pt = boost::posix_time;

cJoystick::cJoystick(int selected_joystick)
{

    joy_index   = selected_joystick;   /* Joystick number. */
    num_joys    = -1;            /* number of joysticks. */

    int num_axes    = -1;        /* Number of axes. */
    int num_buttons = -1;        /* Number of buttons. */
    int num_hats    = -1;        /* Number of hats */

    /* SDL initialization */
    // without video (or everything), the joystick events wont work *grrr*
    // without eventthread, no events on my mac deteced *grrr*
    if (SDL_Init(SDL_INIT_EVERYTHING  | SDL_INIT_EVENTS) < 0)
    {
        cerr << "Could not initialize SDL: " <<  SDL_GetError() << endl;
        exit(1);
    }
    num_joys = SDL_NumJoysticks();
    if (num_joys <= 0)
    {
        cerr << "Sorry, no joysticks found." << endl;
        return;
    }

    /* Get parameter */
    if (joy_index >= num_joys)
    {
        cerr << "Only " << num_joys
             << " joystick(s) were found. Indexing starts at zero. Joystick "
             << joy_index << " does not exist."
             << endl;
        return;
    }

    SDL_JoystickEventState(SDL_ENABLE);
    m_pJoystick = SDL_JoystickOpen(joy_index);

    /* Collect joystick data. */
    num_axes    = SDL_JoystickNumAxes(m_pJoystick);
    num_buttons = SDL_JoystickNumButtons(m_pJoystick);
    num_hats    = SDL_JoystickNumHats(m_pJoystick);
#if defined(DEBUG)
    cout << "Joystick number: " << joy_index
         << ", num_axes: " << num_axes
         << ", num_buttons: " << num_buttons
         << ", num_hats: " << num_hats
         << endl;
#endif
    vf_axis.resize(num_axes);
    vf_button.resize(num_buttons);
    vf_hat.resize(num_hats);

    m_thread_running = true;
    m_joy_thread = boost::thread( boost::bind(&cJoystick::loop, this));

}

void cJoystick::loop(void)
{

    unsigned int ii = 0;

    while(m_thread_running)
    {
//        while(SDL_PollEvent(&event))
//        {
//            switch(event.type)
//            {
//
//            cout << "Event.Type: " << event.type << endl;
//
//            case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
//                if ( ( event.jaxis.value < -3200 ) || (event.jaxis.value > 3200 ) )
//                {
//                    for (ii = 0; ii < vf_axis.size(); ii++)
//                    {
//                        if( ii == event.jaxis.axis)
//                        {
//                            vf_axis[ii] = SDL_JoystickGetAxis(m_pJoystick, ii) / 32768.0;
//                            signal_axis(getJoystick(), event.jaxis.axis);
//                        }// if
//                    }// for
//                }// axis event
//                break;
//
//            case SDL_JOYBUTTONDOWN:  /* Handle Joystick Button Presses */
//                for (ii = 0; ii < vf_button.size(); ii++)
//                {
//                    if( ii == event.jbutton.button)
//                    {
//                        vf_button[ii] = SDL_JoystickGetButton(m_pJoystick, ii);
//                        signal_button(getJoystick(), event.jbutton.button);
//                    }// if
//                }// for
//                break;
//            case SDL_QUIT:
//                m_thread_running = false;
//                break;
//            default:
//                break;
//            } // switch
//        }//SDL_event

        /* Grab current joystick information. */
        SDL_JoystickUpdate();

        unsigned int ii = 0;
        for (ii = 0; ii < vf_axis.size(); ii++)
        {
            vf_axis[ii] = SDL_JoystickGetAxis(m_pJoystick, ii) / 32768.0;
        }

        for (ii = 0; ii < vf_button.size(); ii++)
        {
            vf_button[ii] = SDL_JoystickGetButton(m_pJoystick, ii);
        }

        for (ii = 0; ii < vf_hat.size(); ii++)
        {
            vf_hat[ii] = SDL_JoystickGetHat(m_pJoystick, ii);
        }

        boost::this_thread::sleep(pt::milliseconds(10)); //around 100 Hz
    }// thread_running

}


int cJoystick::getNumberOfButtons ()
{
    return(vf_button.size());
}

int cJoystick::getNumberOfAxes ()
{
    return(vf_axis.size());
}

//const char *cJoystick::getName()
//{
//    return(SDL_JoystickName(joy_index));
//}

const cJoystick* cJoystick::getJoystick()
{
    return( (const cJoystick*)m_pJoystick );
}

float cJoystick::getAxis(int axis)
{
    if(axis > 0 && axis < (int)vf_axis.size())
    {
        return( vf_axis[axis] );
    }
    else
        return(0.);
}

int cJoystick::getButton(int button)
{
    if(button > 0 && button < (int)vf_button.size())
    {
        return( vf_button[button] );
    }
    else
        return(-1);
}


cJoystick::~cJoystick()
{
    m_thread_running = false;
    m_joy_thread.join();

    SDL_JoystickClose(m_pJoystick);
    SDL_Quit();
}
