//#include "Constrain.h"
#include "ControlSystem.h"

#ifndef constrain
    #define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

ControlSystem::ControlSystem(){
  f_Kp = 0.0;
  f_Ki = 0.0;
  f_Kd = 0.0;
  f_integrated_error = 0.0;
  f_last_error = 0.0;
  f_ErrorMin = -1.57; //-pi/2
  f_ErrorMax =  1.57; //pi/2
  f_ServoMin = 900;
  f_ServoMax = 2100;
}


void ControlSystem::setGainKp(float Kp){

  this->f_Kp = Kp;
}

float ControlSystem::getGainKp(void){

  return(f_Kp);
}

void ControlSystem::setGainKi(float Ki){

  this->f_Ki = Ki;
}


float ControlSystem::getGainKi(void){

  return(f_Ki);
}

void ControlSystem::setGainKd(float Kd){

  this->f_Kd = Kd;
}


float ControlSystem::getGainKd(void){

  return(f_Kd);
}

void ControlSystem::setServoMinMax(float f_min, float f_max){
  this-> f_ServoMin =  f_min;
  this-> f_ServoMax =  f_max;
}

void ControlSystem::setErrorMinMax(float f_min, float f_max){
  f_ErrorMin = f_min;
  f_ErrorMax = f_max;
}


float ControlSystem::getControl(float f_cmd, float f_state, float f_offset){

  float f_ctrl_err = constrain( (f_cmd - f_state), f_ErrorMin, f_ErrorMax);
  //proportional
  float f_pTerm = f_Kp * f_ctrl_err;
  //integral
  f_integrated_error = constrain( (f_integrated_error + f_ctrl_err) , f_ErrorMin, f_ErrorMax);
  float f_iTerm = f_Ki * f_integrated_error;
  //differential
  float f_dTerm = f_Kd * (f_ctrl_err - f_last_error);
  f_last_error = f_ctrl_err;
  //PID control
  return( constrain( (float)((f_pTerm + f_iTerm + f_dTerm) + f_offset) , f_ServoMin, f_ServoMax) );
}

