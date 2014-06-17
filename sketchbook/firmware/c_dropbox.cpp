#include "c_dropbox.h"

#include "crc16.h"

cDropbox::cDropbox()
{
  m_load = 0;

  // initialize message buffer for sending pilot commands and status
  // to the raspi
  memset(buffer, 0, sizeof(buffer));
  buffer[0] = 0xff;
  buffer[1] = 0xfe;
}

void cDropbox::set_channels(cServoChannels channel)
{
  if(channel.valid())
  {
    //    channel_1_rud = 1601;
    //    channel_2_thr = 1602;
    //    channel_3_ele = 1603;
    //    channel_4_ail = 1604;
    //channel[tbd_5] = 1234;
    //    channel_6_mode= 1606;

    // fill buffer with pilots inputs
    buffer[2] = (channel[rud_1]>>8) & 0xff;
    buffer[3] =  channel[rud_1] & 0xff  ;

    buffer[4] = (channel[thr_2]>>8) & 0xff;
    buffer[5] =  channel[thr_2] & 0xff  ;

    buffer[6] = (channel[ele_3]>>8) & 0xff;
    buffer[7] =  channel[ele_3] & 0xff  ;

    buffer[8] = (channel[ail_4]>>8) & 0xff;
    buffer[9] =  channel[ail_4] & 0xff  ;

    buffer[10] = (channel[tbd_5]>>8) & 0xff;
    buffer[11] =  channel[tbd_5] & 0xff  ;

    buffer[12] = (channel[mod_6]>>8) & 0xff;
    buffer[13] =  channel[mod_6] & 0xff  ;
  }
}

void cDropbox::set_mode(void)
{
}

void cDropbox::set_voltage(void)
{
}

void cDropbox::set_load(uint8_t load)
{
  m_load=load;
}

void cDropbox::set_errors(uint32_t errors)
{
  m_errors = errors;
}

void cDropbox::send_message()
{
  // calculate checksum for channel data
  unsigned short crc = crcFast(&buffer[2], 12); // checksum of 12 payload bytes

  buffer[14] =  (crc>>8) & 0xff;
  buffer[15] =  crc & 0xff;

  // transmit data to raspi
  Serial.write((uint8_t*)&buffer, 16); 
}


