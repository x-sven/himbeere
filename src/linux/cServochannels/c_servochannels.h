#ifndef SERVO_CHANNELS_H
#define SERVO_CHANNELS_H

#if !defined(__linux__)  && !defined(__APPLE__)
#include <Arduino.h>
#else
#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */
#include <iostream>
#include <string.h>
#include <stdint.h>
#define boolean bool
#endif

enum eChannel {rud_1=0, thr_2, ele_3, ail_4, tbd_5, mod_6, _num_channels};

class cServoChannels
{
  public:
    cServoChannels(size_t size);
    ~cServoChannels();
    void setChannel(size_t no, uint16_t value);
    uint16_t getChannel(size_t no);
    boolean valid();
    size_t size(void);
    cServoChannels & operator=(cServoChannels const &other);

  private:
    size_t m_size;
    uint16_t *p_channel;
    const uint16_t ppm_min;
    const uint16_t ppm_max;
};

#endif //SERVO_CHANNELS_H
