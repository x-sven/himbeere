#include <iostream>
#include <fstream>
#include <signal.h>

#include "HardwareSerial/HardwareSerial.h"

#include "Millis/millis.h"

using namespace std;

//HardwareSerial Serial("/dev/ttyUSB0");
HardwareSerial Serial("/dev/ttyAMA0");

bool execute = true;

//***************************************
void trap(int signal)
{
    cout << "\n SIGINT received!" << endl;
    execute = false;
}


//---------------------------------------------------------
void setup()
{
    Serial.begin(38400);


}

int main()
{
    uint8_t bytedata;
    int16_t numc;
    uint8_t reciever_state=0;
    uint8_t reciever_couter=0;

    setup();

    // register Ctrl-C signal
    signal(SIGINT, &trap);


    uint16_t rx_channel[6];

    while(execute)
    {

        numc = Serial.available();
        if(0 < numc )
        {
           // printf("Number bytes: %d \n",numc); fflush(stdout);
            for (int16_t i=0; i < numc; i++)
            {

                bytedata = Serial.read();
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

                    if(bytedata==0xfe ){
                        reciever_couter=0;
                        reciever_state=0;
                    }
                    if(bytedata==0xff){
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
//                    printf("%d %d %d %d %d %d\n",rx_channel[0],rx_channel[1],rx_channel[2],rx_channel[3],rx_channel[4],rx_channel[5]);
                    reciever_state=0;
                    break;
                default:
                    ;

                }

            }//for

        }//if

        //printf("0x%d\n", data);

        usleep(1000);
    }
    Serial.end();
    return(0);
}
