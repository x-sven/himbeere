#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>

#define private public

#include "cServochannels/c_parser.h"
#include "cServochannels/c_servochannels.h"
#include "crc16/crc16.h"

using namespace std;


void test_messageparser(void)
{

    unsigned char message[] = {0xFF, 0xFE, // sync
                               0x00, 0x01, // ch 1
                               0x00, 0x02,
                               0x00, 0x03,
                               0x00, 0x04, // ch4
                               0x00, 0x00}; //crc

    cParser parser;

    BOOST_CHECK(0 == parser.step);
    BOOST_CHECK(0 == parser.payload_counter);
    for(uint8_t ii=0; ii<10; ii++)
        BOOST_CHECK(0 == parser.rx_message[ii]);

    parser.process_byte(message[0]); // FF
        BOOST_CHECK(1 == parser.step);
    parser.process_byte(message[1]); // FE
        BOOST_CHECK(2 == parser.step);

    for(int ii=2; ii< 10; ii++)
    {
        parser.process_byte(message[ii]);
        BOOST_CHECK(message[ii] == parser.rx_message[ii-2]); // channel bytes
    }

    BOOST_CHECK_EQUAL(parser.payload_counter, 8);

    unsigned short crc = crcFast(&message[2], 8); // checksum of 8 payload bytes

    BOOST_CHECK_EQUAL(crcFast(&parser.rx_message[0],8), crcFast(&message[2], 8));

    message[10] =  (crc>>8) & 0xff; // swap!?
    message[11] =  crc & 0xff;

    parser.process_byte(message[10]); // crc
        BOOST_CHECK(message[10] == parser.rx_message[8]);
        BOOST_CHECK_EQUAL(parser.payload_counter, 9);
    parser.process_byte(message[11]);
        BOOST_CHECK(message[11] == parser.rx_message[9]);
        BOOST_CHECK_EQUAL(parser.payload_counter, 0); // counter is set to 0 above 9

    BOOST_CHECK_EQUAL(crcFast(&parser.rx_message[0],8),crcFast(&message[2], 8));

    BOOST_CHECK_EQUAL(parser.b_new, true);
    BOOST_CHECK_EQUAL(parser.parse_errors, 0);
    BOOST_CHECK_EQUAL(parser.step, 0);
    BOOST_CHECK_EQUAL(parser.payload_counter, 0);

    cServoChannels channel_raspi_6(6);

    for(size_t ii=0; ii<channel_raspi_6.size();ii++)
        BOOST_CHECK_EQUAL(channel_raspi_6.getChannel(ii), 0);

    // get channels from parser
    if(parser.new_channels())
        channel_raspi_6 = parser.get_channels(); // channel_*_6 has now only 4 elements!

    for(size_t ii=0; ii<channel_raspi_6.size();ii++)
        BOOST_CHECK_EQUAL(channel_raspi_6.getChannel(ii), ii+1);
}
