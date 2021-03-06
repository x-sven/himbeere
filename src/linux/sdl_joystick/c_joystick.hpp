#ifndef CJOYSTICK_HPP
#define CJOYSTICK_HPP

#include "SDL.h"
#include <boost/thread.hpp>
#include <boost/signals2.hpp>

class cJoystick
{
public:

    cJoystick(int selected_joystick = 0);
    virtual ~cJoystick();

    int getNumberOfButtons ();
    int getNumberOfAxes ();
    //const char* getName();
    const cJoystick* getJoystick();
    float getAxis(uint8_t axis);
    int getButton(uint8_t button);


    boost::signals2::signal<void (const cJoystick*, int axis  )>  signal_axis;
    boost::signals2::signal<void (const cJoystick*, int button)>  signal_button;

protected:
private:
    SDL_Joystick *m_pJoystick; /* The current joystick. */
    SDL_Event event;

    std::vector<float>  vf_axis;
    std::vector<int>    vf_button;
    std::vector<float>  vf_hat;

    int joy_index;       /* Joystick number. */
    int num_joys;        /* number of joysticks. */

    boost::thread m_joy_thread;
    bool m_thread_running;

    void loop(void);

};

#endif // CJOYSTICK_HPP
