#include <iostream>
#include <fstream>
#include <signal.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Millis/millis.h"

#include "HardwareSerial/HardwareSerial.h"
#include "ServoInterface/ServoInterface.h"
#include "ControlSystem/ControlSystem.h"

using namespace std;

namespace pt = boost::posix_time;

#if defined(__arm__)
HardwareSerial Serial("/dev/ttyAMA0");
#else
HardwareSerial Serial("/dev/ttyUSB0");
#endif // defined

ServoInterface fcci;

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
        // *********** 100Hz begin ***********
        fcci.update();

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
