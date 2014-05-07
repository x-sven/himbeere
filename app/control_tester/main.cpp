#include <iostream>
#include <fstream>
#include <signal.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Millis/millis.h"

#include "HardwareSerial/HardwareSerial.h"
#include "ServoInterface/ServoInterface.h"
#include "ControlSystem/ControlSystem.h"

#include "crc16.h"

using namespace std;

namespace pt = boost::posix_time;

#if defined(__arm__)
HardwareSerial Serial("/dev/ttyAMA0");
#else
HardwareSerial Serial("/dev/ttyUSB0");
#endif // defined

ServoInterface fcci;

bool execute = true;

uint8_t command_state;
uint8_t command_buffer[12];


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

    memset(command_buffer,0,12);
    command_buffer[0] = 0xff;
    command_buffer[1] = 0xfe;
}

void set_commands(uint16_t ped_us, uint16_t col_us, uint16_t lon_us, uint16_t lat_us)
{
    unsigned int ppm_min=900;
    unsigned int ppm_max=2100;

    if(lat_us<ppm_min || lat_us >ppm_max)
            command_state=1;
    if(lon_us<ppm_min || lon_us >ppm_max)
            command_state=1;
    if(ped_us<ppm_min || ped_us >ppm_max)
            command_state=1;
    if(col_us<ppm_min || col_us >ppm_max)
            command_state=1;


    command_buffer[2] = (ped_us>>8) & 0xff;
    command_buffer[3] = ped_us & 0xff  ;

    command_buffer[4] = (col_us>>8) & 0xff;
    command_buffer[5] = col_us & 0xff  ;

    command_buffer[6] = (lon_us>>8) & 0xff;
    command_buffer[7] = lon_us & 0xff  ;

    command_buffer[8] = (lat_us>>8) & 0xff;
    command_buffer[9] = lat_us & 0xff  ;


    unsigned short crc = crcsum(&command_buffer[2], 8, 0xffff);

    command_buffer[10] =  crc & 0xff;
    command_buffer[11] =  (crc>>8) & 0xff;


    command_buffer[10] =   0xcc;
    command_buffer[11] =   0xcc;

    Serial.write((uint8_t*)&command_buffer, 12);
}



//---------------------------------------------------------
int main()
{
    setup();

    // register Ctrl-C signal
    signal(SIGINT, &trap);

    pt::time_duration interval_100Hz(pt::milliseconds(1000 / 100));
    pt::ptime timer_100Hz = pt::microsec_clock::local_time() + interval_100Hz;

    pt::time_duration interval_10Hz(pt::milliseconds(1000 / 10));
    pt::ptime timer_10Hz = pt::microsec_clock::local_time()  + interval_10Hz;

    ControlSystem CtrlLoop;
    CtrlLoop.setGainKp(1.0);

//    float yr = 0;
//    const float mainamplitude   = 1.; // us
//    float mainfreq              = 0* 0.1 *6.2831853;  // Hz = 2*pi*rad
//    float chirpfreq             = 0.1 *6.2831853;  // (Delta_f)/Delta_t ->  5Hz/10s
//    float chirpamplitude        = 1   *6.2831853;  // Hz
    pt::ptime initial_microseconds = pt::microsec_clock::local_time();

    while(execute)
    {
        //   set_commands(1900,1900,1900,1900);

        // *********** 100Hz begin ***********
        if(fcci.update()){
            set_commands(fcci.get_channel(0),fcci.get_channel(1),fcci.get_channel(2),fcci.get_channel(3));
            //set_commands(1900,1900,1900,1900);

        }

//        float f_seconds_now = ((pt::microsec_clock::local_time()-initial_microseconds).total_microseconds()/1.e+6);
//        float frequency = mainfreq + (chirpamplitude/2.*cos(chirpfreq*f_seconds_now-3.1415/2.0) + chirpamplitude/2.);
//        yr = mainamplitude*sin(frequency*f_seconds_now);
//
//        printf("%.3f %.3f\n", f_seconds_now, yr);

        // ******** 10Hz begin ********
        if (timer_100Hz > timer_10Hz)
        {
            printf("Servo channels: ");
            for(uint8_t ch = 0; ch < fcci.num_channel_max; ch++)
            {
                printf("%d ", fcci.get_channel(ch));
            }
            printf("\n");

            set_commands(0,0,0,0);

            timer_10Hz +=  interval_10Hz;
        }//10 Hz
        // ******** 10Hz end ********

        pt::time_duration spare_time = (timer_100Hz - pt::microsec_clock::local_time()); //spare time
        pt::time_duration working_time = interval_100Hz-spare_time;
        if (spare_time.is_negative())
        {
            std::cout << pt::microsec_clock::local_time()
                      << ": "
                      << "Warning: control_tester overrun! (+"
                      << (working_time-interval_100Hz).total_microseconds()
                      << " us)" << std::endl;
        }
        boost::this_thread::sleep(timer_100Hz - pt::microsec_clock::local_time());
        timer_100Hz += interval_100Hz;
        // *********** 100Hz end ***********

    }
    Serial.end();
    return(0);
}
