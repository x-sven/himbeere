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
    if (SDL_Init(SDL_INIT_JOYSTICK) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
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

    m_pJoystick = SDL_JoystickOpen(joy_index);

    if (m_pJoystick == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_JoystickOpen failed: %s\n",
                     SDL_GetError());
    }
    else
    {
        char guid[64];
        SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(m_pJoystick),
                                  guid, sizeof (guid));
        SDL_Log("Joystick found!");
        SDL_Log("       axes: %d\n", SDL_JoystickNumAxes(m_pJoystick));
        SDL_Log("      balls: %d\n", SDL_JoystickNumBalls(m_pJoystick));
        SDL_Log("       hats: %d\n", SDL_JoystickNumHats(m_pJoystick));
        SDL_Log("    buttons: %d\n", SDL_JoystickNumButtons(m_pJoystick));
        SDL_Log("instance id: %d\n", SDL_JoystickInstanceID(m_pJoystick));
        SDL_Log("       guid: %s\n", guid);
    }



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
    while(m_thread_running)
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
                vf_axis.at(event.jaxis.axis) = (float)event.jaxis.value/32768.; // scaling to +/-1
                signal_axis(getJoystick(), event.jaxis.axis);
                break;
            case SDL_JOYBUTTONDOWN:  /* Handle Joystick Button Presses */
                vf_button.at(event.jbutton.button) = (int)event.jbutton.state;
                signal_button(getJoystick(), event.jbutton.button);
                break;
            case SDL_QUIT:
                m_thread_running = false;
                break;
            default:
                break;
            } // switch
        }//SDL_event
//
//        /* Grab current joystick information. */
//        SDL_JoystickUpdate();
//
//        unsigned int ii = 0;
//        for (ii = 0; ii < vf_axis.size(); ii++)
//        {
//            vf_axis[ii] = SDL_JoystickGetAxis(m_pJoystick, ii) / 32768.0;
//        }
//
//        for (ii = 0; ii < vf_button.size(); ii++)
//        {
//            vf_button[ii] = SDL_JoystickGetButton(m_pJoystick, ii);
//        }
//
//        for (ii = 0; ii < vf_hat.size(); ii++)
//        {
//            vf_hat[ii] = SDL_JoystickGetHat(m_pJoystick, ii);
//        }
//
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

float cJoystick::getAxis(uint8_t axis)
{
    if(axis < (uint8_t)vf_axis.size())
    {
        return( vf_axis.at(axis) );
    }
    else
        return(99.);
}

int cJoystick::getButton(uint8_t button)
{
    if(button < (int)vf_button.size())
    {
        return( vf_button.at(button) );
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
