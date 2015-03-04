#include <iostream>
#include <fstream>
#include <signal.h>

#include "HardwareSerial/HardwareSerial.h"

//#include "millis.h"

using namespace std;

HardwareSerial Serial("/dev/tty.usbmodem411");
//HardwareSerial Serial("/dev/ttyAMA0");

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
    Serial.begin(115200);


}

int main()
{

    int incomingByte = 0;   // for incoming serial data

    setup();

    // register Ctrl-C signal
    signal(SIGINT, &trap);

    while(execute)
    {

        //numc = Serial.available();
        //            bytedata = Serial.read();
        //printf("0x%d\n", data);


        // send data only when you receive data:
        if (Serial.available() > 0)
        {

            for(int ii=0; ii<Serial.available(); ii++){
                //cout << "Serial.available() returned: " << Serial.available() << endl;
                // read the incoming byte:
                incomingByte = Serial.read();

                // say what you got:
                printf("%c", incomingByte);
            }
        }

        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    }

    Serial.end();
    return(0);
}
