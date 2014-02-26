// Control.h
//This Class calculates the contol signals

#ifndef _Control_H_
#define _Control_H_


class ControlSystem
{
  public:
        ControlSystem(); //Constructor
        void   setGainKp(float Kp);
        float  getGainKp(void);
        void   setGainKi(float Ki);
        float  getGainKi(void);
        void   setGainKd(float Kd);
        float  getGainKd(void);
        void   setServoMinMax(float i_min, float i_max);
        void   setErrorMinMax(float f_min, float f_max);
        float  getServoCmd(float cmd, float state, float i_offset);

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
