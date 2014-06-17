#ifndef DROPBOX_H
#define DROPBOX_H

#include <Arduino.h>
#include "c_servochannels.h"

class cDropbox
{
public:
  cDropbox();
  void set_channels(cServoChannels channels);
  void set_mode(void);
  void set_voltage(void);
  void set_load(uint8_t load);
  void set_errors(uint32_t load);
  void send_message(void);

private:
  uint8_t buffer[16];     // buffer for message send to raspi, holding e.g. pilot's pulse width
  
  uint8_t m_load;
  uint32_t m_errors;

};



#endif //DROPBOX_H


