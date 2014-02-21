#include <iostream>
#include <fstream>
#include <signal.h>


#include "Millis/millis.h"

#include "HardwareSerial/HardwareSerial.h"
#include "ServoInterface/ServoInterface.h"

using namespace std;

//HardwareSerial Serial("/dev/ttyUSB0");
HardwareSerial Serial("/dev/ttyAMA0");
cServoInterfaceClass fcci;

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
    fcci.begin(&Serial);
}

//---------------------------------------------------------
int main()
{
    setup();

    // register Ctrl-C signal
    signal(SIGINT, &trap);

    while(execute)
    {

        fcci.update();

        for(uint8_t ch = 0; ch < fcci.num_channel_max; ch++)
        {
            printf("%d ", fcci.get_channel(ch));
        }
        printf("\n");


        //printf("0x%d\n", data);
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    }
    Serial.end();
    return(0);
}
