#include <iostream>
#include <fstream>
#include <netinet/in.h>

#include <boost/thread.hpp>

#include "HardwareSerial/HardwareSerial.h"
#include "AP_GPS.h"

#include "millis.h"
#include "Logging/Logging.h"

using namespace std;

//HardwareSerial Serial("/dev/ttyUSB0");
HardwareSerial Serial("/dev/ttyAMA0");

//boost::thread receiving_thread;




AP_GPS_MTK16 gps(&Serial);

Logging gpslog;


//int little_endian(){
//    int x = 1;
//    return *(char*)&x;
//}
//int big_endian(){
//    return !little_endian();
//}


// struct diyd_mtk_msg
// {
//        int32_t latitude;
//        int32_t longitude;
//        int32_t altitude;
//        int32_t ground_speed;
//        int32_t ground_course;
//        uint8_t satellites;
//        uint8_t fix_type;
//        uint32_t utc_date;
//        uint32_t utc_time;
//        uint16_t hdop;
//};


//---------------------------------------------------------
void setup()
{
    Serial.begin(38400);

    gpslog.begin("mtk_logfile.txt");
    gpslog.write("# Headline missing...\n");

    stderr = stdout;
    gps.print_errors = true;

    //Serial.println("GPS MTK library test");
    gps.init();  // GPS Initialization
    sleep(1);
}

int main()
{

//diyd_mtk_msg msg;
//
//    memset(&msg, 0, sizeof(msg));
//
//    msg.latitude  = 0x01020304;
//    msg.longitude = 0x01020304;
//    msg.altitude  = 0x01020304;
//    msg.ground_speed  = 0x01020304;
//    msg.ground_course = 0x01020304;
//    msg.satellites = 0x01;
//    msg.fix_type = 0x01;
//    msg.utc_date = 0x01020304;
//    msg.utc_time = 0x01020304;
//    msg.hdop     = 0x0102;
//
//    char *pmsg = (char*)&msg;
//
//  ofstream myfile;
//  myfile.open ("debug.txt");
//
//    for(unsigned int ii = 0; ii < sizeof(msg); ii++)
//        myfile << std::hex << pmsg[ii];
//
//    myfile.close();
//
//exit(1);

    setup();

//    for(int ii = 0; ii< 100; ii++)
//    {
//        std::cout << millis() << "(" << ii << ")" << " I'm the main_loop()!, chars available: "
//        << Serial.available() << std::endl;
//        usleep(1000);
//    }
    unsigned long start = millis();

//uint32_t value = 0x4030201;
//std::cout << "value: " << value << std::endl;
//
////correct
//std::cout << "left shifted bytes: "
//              << (uint32_t)(0x4<<24 | 0x3<<16 | 0x2<<8 | 0x01)
//              << std::endl;
////wrong
//std::cout << "ntohl shifted bytes (4321): "
//              << ntohl((uint32_t)(0x4<<24 | 0x3<<16 | 0x2<<8 | 0x01))
//              << std::endl;
////correct: big -> little
//std::cout << "ntohl shifted bytes (1234): "
//              << ntohl((uint32_t)(0x1<<24 | 0x2<<16 | 0x3<<8 | 0x04))
//              << std::endl;


//    if(big_endian())
//        printf("big_endian\n");
//    if(little_endian())
//        printf("little_endian\n");



    while(1)
    {
        //std::cout << "Main, Bytes: " << Serial.available() << std::endl;
        gps.update();

        if (gps.new_data)
        {
            std::cout <<"----GPS----"                    << std::endl;
            std::cout <<" Lat: "
                      << (float)gps.latitude/1.e+7       << std::endl;
            std::cout <<" Lon: "
                      << (float)gps.longitude/1.e+7      << std::endl;
            std::cout <<" Alt: "
                      << (float)gps.altitude / 100.0     << std::endl;
            std::cout <<" Speed: "
                      << (float)gps.ground_speed / 100.0 << std::endl;
            std::cout <<" Course: "
                      << (float)gps.ground_course / 100.0<< std::endl;
            std::cout <<" SAT: "
                      << (int)gps.num_sats               << std::endl;
            std::cout <<" FIX: "
                      << (int)gps.fix                    << std::endl;
            std::cout <<" HDOP: "
                      << (float)gps.hdop / 100.0         << std::endl;
            std::cout <<" TIME: "
                      << gps.time                        << std::endl;
            std::cout <<" DATE: "
                      << gps.date                        << std::endl;

            gps.new_data = 0; // We have read the data
        }
        std::cout << "-----------\n Runtime: " << 0.001*(double)(millis()-start) << std::endl;
        usleep(200000); //200ms
    }
    Serial.end();
    gpslog.end();
    return(0);
}
