#ifndef PARSER_H
#define PARSER_H

#if !defined(__linux__)  && !defined(__APPLE__)
#include <Arduino.h>
#else
#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */
#include <iostream>
#include <string.h>
#include <stdint.h>
#define boolean bool
#define byte unsigned char
#endif

#include "c_servochannels.h"

class cParser
{
public:
  cParser();
  void process_byte(byte rx_byte);
  uint32_t get_errors(void);
  void reset_errors(void);
  boolean new_channels()  {return b_new;}
  cServoChannels& get_channels();


private:
  unsigned char step;              // step of serial data parser
  unsigned char payload_counter;   // counter for serial data
  unsigned char rx_message[10];    // char buffer for (complete) serial message
  uint32_t parse_errors;   // counter for serial recieve errors
  cServoChannels *pchannels;
  boolean b_new;
};



#endif //PARSER_H


