#include "c_joystick_manager.hpp"

using namespace std;

#ifndef constrain
    #define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

cJoystickManager::cJoystickManager():
    joy(NULL)
{
    bool fail = false;
    //ctor
    joy = new cJoystick(0);

    if(NULL == joy)
        fail = true;
//    else if(NULL == joy->getName())
//        fail = true;

    if(fail)
        cout << "cJoystickManager: Could not initialize joystick!" << endl;
    else
    {
        for(int ii = 0; ii < _eCounter; ii++)
        {
            m_channel.push_back(m_proberties());
            this->setProberty(eChannel(ii),ii, 1, 1, 0, -1, 0);
            cout << "axis: " << (int)ii << endl;
        }
    }
}

cJoystickManager::~cJoystickManager()
{
    //dtor
    if(NULL != joy)
        delete(joy);
}

void cJoystickManager::setProberty(eChannel e_ch, uint8_t axis, float scale, float upper, float offset, float lower, float failsafe)
{
    m_channel.at(e_ch).axis = axis;
    m_channel.at(e_ch).scale = scale;
    m_channel.at(e_ch).upper = upper;
    m_channel.at(e_ch).lower = lower;
    m_channel.at(e_ch).offset = offset;
    m_channel.at(e_ch).failsafe = failsafe;
}

float cJoystickManager::getScaledAxis(eChannel e_ch)
{
    float unscaled = joy->getAxis(m_channel.at(e_ch).axis);
    float scaled   = (unscaled*m_channel.at(e_ch).scale) + m_channel.at(e_ch).offset;
    float limited  =  constrain(scaled, m_channel.at(e_ch).lower, m_channel.at(e_ch).upper);

    return(limited);
}

void cJoystickManager::setRollProberty(uint8_t axis, float scale, float upper, float offset, float lower, float failsafe)
{
    setProberty(eRoll, axis, scale, upper, offset, lower, failsafe);
}

void cJoystickManager::setPitchProberty(uint8_t axis, float scale, float upper, float offset, float lower, float failsafe)
{
    setProberty(ePitch, axis, scale, upper, offset, lower, failsafe);
}

void cJoystickManager::setYawProberty(uint8_t axis, float scale, float upper, float offset, float lower, float failsafe)
{
    setProberty(eYaw, axis, scale, upper, offset, lower, failsafe);
}

void cJoystickManager::setColProberty(uint8_t axis, float scale, float upper, float offset, float lower, float failsafe)
{
    setProberty(eCol, axis, scale, upper, offset, lower, failsafe);
}

void cJoystickManager::setModeProberty(uint8_t axis, float scale, float upper, float offset, float lower, float failsafe)
{
    setProberty(eMode, axis, scale, upper, offset, lower, failsafe);
}

float cJoystickManager::getRollValue()
{
    return( getScaledAxis(eRoll) );
}

float cJoystickManager::getPitchValue()
{
    return( getScaledAxis(ePitch) );
}

float cJoystickManager::getYawValue()
{
    return( getScaledAxis(eYaw) );
}

float cJoystickManager::getColValue()
{
    return( getScaledAxis(eCol) );
}

float cJoystickManager::getModeValue()
{
    return( getScaledAxis(eMode) );
}
