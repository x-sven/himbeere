// Control.h
//This Class calculates a control signals

#ifndef _Control_H_
#define _Control_H_

#include "ControlSystem/i_CTRL.h"

class ControlSystem: public iCTRL
{
  public:
        ControlSystem(); //Constructor
        void   setGainKp(float Kp);
        float  getGainKp(void);
        void   setGainKi(float Ki);
        float  getGainKi(void);
        void   setGainKd(float Kd);
        float  getGainKd(void);
        void   setServoMinMax(float f_min, float f_max);
        void   setErrorMinMax(float f_min, float f_max);
        float  getControl(float cmd, float state, float offset);


  private:
        float f_Kp;
        float f_Ki;
        float f_Kd;
        float f_integrated_error;
        float f_last_error;
        float f_ErrorMin, f_ErrorMax;
        float f_ServoMin, f_ServoMax;
};


#endif //_Control_H_
