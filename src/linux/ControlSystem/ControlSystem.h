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
        float* Kp(void){return &f_Kp;}

        void   setGainKi(float Ki);
        float  getGainKi(void);
        float* Ki(void){return &f_Ki;}

        void   setGainKd(float Kd);
        float  getGainKd(void);
        float* Kd(void){return &f_Kd;}

        void   setControlMinMax(float f_min, float f_max);
        float*   ControlMin(void){return &f_ControlMin; }
        float*   ControlMax(void){return &f_ControlMax;}

        void   setErrorMinMax(float f_min, float f_max);
        float*   ErrorMin(void){return &f_ErrorMin; }
        float*   ErrorMax(void){return &f_ErrorMax;}

        float  getControl(float cmd, float state, float offset);


  private:
        float f_Kp;
        float f_Ki;
        float f_Kd;
        float f_integrated_error;
        float f_last_error;
        float f_ErrorMin, f_ErrorMax;
        float f_ControlMin, f_ControlMax;
};


#endif //_Control_H_
