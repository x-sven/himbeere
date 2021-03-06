#ifndef CGAMECONTROLLER_HPP
#define CGAMECONTROLLER_HPP

#include "SDL.h"
#include <boost/thread.hpp>
#include <boost/signal.hpp>

class cJoystick
{
public:

    cJoystick(int selected_joystick = 0);
    virtual ~cJoystick();

    int getNumberOfButtons ();
    int getNumberOfAxes ();
    //const char* getName();
    const cJoystick* getJoystick();
    float getAxis(int axis);
    int getButton(int button);


    boost::signal<void (const cJoystick*, int axis  )>  signal_axis;
    boost::signal<void (const cJoystick*, int button)>  signal_button;

protected:
private:
    SDL_GameController *m_pJoystick; /* The current joystick. */
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

#endif // CGAMECONTROLLER_HPP
