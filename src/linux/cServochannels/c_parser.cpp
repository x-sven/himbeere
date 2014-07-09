#include "c_parser.h"

#include "crc16/crc16.h"


cParser::cParser(void):
    step(0),
    payload_counter(0),
    parse_errors(0),
    b_new(false)
{
    for(uint8_t ii=0; ii<10; ii++)
        rx_message[ii] = 0;

    pchannels = (cServoChannels*)new cServoChannels(4);

        for(size_t ii=0; ii<pchannels->size(); ii++)
        pchannels->setChannel(ii, ii);
}

void cParser::process_byte(byte rx_byte)
{
    switch(step)
    {
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
        rx_message[payload_counter++]=rx_byte;
        if(payload_counter>9) // at least 10 bytes received (8+2)
        {
            unsigned int crc_received = (rx_message[8] << 8) + rx_message[9];
            unsigned int crc_expected = crcFast(rx_message,8);
            if(crc_received == crc_expected)
            {
                pchannels->setChannel( rud_1,(rx_message[0] << 8) + rx_message[1] );
                pchannels->setChannel( thr_2,(rx_message[2] << 8) + rx_message[3] );
                pchannels->setChannel( ele_3,(rx_message[4] << 8) + rx_message[5] );
                pchannels->setChannel( ail_4,(rx_message[6] << 8) + rx_message[7] );
                b_new = true;
            }// crc verified
            else
            {
                parse_errors++; //ToDo: transmit information to raspi
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

