#include "c_joystick_manager.hpp"

using namespace std;

cJoystickManager::cJoystickManager():
    joy(NULL)
{
    bool fail = false;
    //ctor
    joy = new cJoystick();

    if(NULL == joy)
        fail = true;
    else if(NULL == joy->getName())
        fail = true;

    if(fail)
        cout << "cJoystickManager: Could not initialize joystick!" << endl;


}

cJoystickManager::~cJoystickManager()
{
    //dtor
    if(NULL != joy)
        delete(joy);
}
