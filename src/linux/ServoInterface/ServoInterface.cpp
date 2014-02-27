#include "ServoInterface.h"

ServoInterface::ServoInterface(Stream *stream):
    num_channel_max(6),
    m_stream(NULL),
    reciever_state(0),
    reciever_couter(0),
    rx_channel(NULL)
{
    rx_channel = (uint16_t*)malloc(num_channel_max*sizeof(uint16_t));

    memset(rx_channel, 0, 6);
    if(NULL != stream)
    {
        begin(stream);
    }
}

void ServoInterface::begin(Stream *stream)
{
    if(NULL != stream)
    {
        m_stream = stream;
    }
}

void ServoInterface::update(void)
{
    uint8_t bytedata = 0;
    int16_t numc = 0;

    numc = m_stream->available();
    if(0 < numc )
    {
        // printf("Number bytes: %d \n",numc); fflush(stdout);
        for (int16_t i=0; i < numc; i++)
        {

            bytedata = m_stream->read();
            //printf("0x%x ",bytedata);
            switch(reciever_state)
            {

            case 0:
                if(bytedata==(uint8_t)0xff)
                    reciever_state=1;
                else
                {
                    reciever_state=0;
                    reciever_couter=0;
                }
                break;

            case 1:
                if(bytedata==0xfe)
                    reciever_state=2;
                else
                {
                    reciever_state=0;
                    reciever_couter=0;
                }
                break;

            case 2:

                if(reciever_couter%2==0)
                {
                    rx_channel[reciever_couter/2]=bytedata<<8;
                }
                else
                {
                    rx_channel[(unsigned int)reciever_couter/2] |= bytedata;
                }

                reciever_couter++;

                if(bytedata==0xfe )
                {
                    reciever_couter=0;
                    reciever_state=0;
                }
                if(bytedata==0xff)
                {
                    reciever_couter=0;
                    reciever_state=1;
                }


                if(reciever_couter>11) // 6x2 bytes in rx_channel
                {
                    reciever_couter=0;
                    reciever_state=3;
                }
                break;
            case 3:
//                    printf("\n");
//                printf("%d %d %d %d %d %d\n",rx_channel[0],rx_channel[1],rx_channel[2],rx_channel[3],rx_channel[4],rx_channel[5]);

// TODO (sven#1#): CRC check here?

                reciever_state=0;
                break;
            default:
                ;

            }//switch case
        }//for(numc)
    }//if()
}

uint16_t ServoInterface::get_channel(uint8_t channel)
{
    if(0 < channel) // && channel < num_ch_max
    {
        return(rx_channel[channel]);
    }
    else
    {
        return(0);
    }
}


ServoInterface::~ServoInterface()
{
    if(NULL != rx_channel)
    {
        free(rx_channel);
    }
}

