#include "c_parser.h"

#include "crc16.h"


cParser::cParser(void):
  step(0),
  payload_counter(0),
  parse_errors(0),
  b_new(false)
{
  for(uint8_t ii=0; ii<10; ii++)
    rx_message[ii] = 0;
  pchannels = new cServoChannels(4);
}

void cParser::process_byte(byte rx_byte)
{
    switch(step){
    case 0:

      if(rx_byte==0xFF)
      {
        step++;
      }
      else
      {
        step=0;
        payload_counter=0;
      }
      break;
    case 1:
      if(rx_byte==0xFE)
        step=2;
      else
      {
        step=0;
        payload_counter=0;
      }
      break;
    case 2:
      rx_message[payload_counter]=rx_byte;
      payload_counter++;
      if(payload_counter>9) // at least 10 bytes received (8+2)
      {
        if(crcverify(&rx_message[0],10))
        {
          pchannels[rud_1]=(rx_message[0] << 8) + rx_message[1];
          pchannels[thr_2]=(rx_message[2] << 8) + rx_message[3];
          pchannels[ele_3]=(rx_message[4] << 8) + rx_message[5];
          pchannels[ail_4]=(rx_message[6] << 8) + rx_message[7];
          b_new = true;
        }// crc verified
        else
        {
          parse_errors++; //ToDo: transmitt information to raspi
        }
        step=0;
        payload_counter=0;
      }
      break;
    }// switch(step)
}

uint32_t cParser::get_errors(void)
{
  return(parse_errors);
}

void cParser::reset_errors(void)
{
  parse_errors = 0;
}

cServoChannels& cParser::get_channels()
{
  b_new = false;
  return(*pchannels);
}

