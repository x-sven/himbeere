#ifndef SERVO_CHANNELS_H
#define SERVO_CHANNELS_H

#if !defined(__linux__)  && !defined(__APPLE__)
#include <Arduino.h>
#else
#include <iostream>
#define boolean bool
#endif

enum eChannel {rud_1=0, thr_2, ele_3, ail_4, tbd_5, mod_6};

class cServoChannels
{
  public:
    cServoChannels(size_t size);
    ~cServoChannels();
    boolean valid();
    cServoChannels & operator=(cServoChannels const &other);
    unsigned int & operator[](size_t const &index);

  private:
    size_t m_size;
    unsigned int *p_channel;
    const unsigned int ppm_min;
    const unsigned int ppm_max;
};

#endif //SERVO_CHANNELS_H
