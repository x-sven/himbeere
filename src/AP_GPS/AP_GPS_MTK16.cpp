// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: t -*-
//
//  DIYDrones Custom Mediatek GPS driver for ArduPilot and ArduPilotMega.
//	Code by Michael Smith, Jordi Munoz and Jose Julio, DIYDrones.com
//
//	This library is free software; you can redistribute it and / or
//	modify it under the terms of the GNU Lesser General Public
//	License as published by the Free Software Foundation; either
//	version 2.1 of the License, or (at your option) any later version.
//
//	GPS configuration : Custom protocol per "DIYDrones Custom Binary Sentence Specification V1.1"
//
//  Modified by Sven Lorenz (2013)
//  - direct access to serial IF via Serial.print removed
//  - Parser modified to support big and little endian

//#include <FastSerial.h>
#include "AP_GPS_MTK16.h"
#include <stdint.h>

#if defined(GPS_DEBUGGING)
    #if defined(WITH_STDIO)
        #define Debug(fmt, args ...)  do {printf("AP_GPS_MTK16::%s:%d: " fmt "\n", __FUNCTION__, __LINE__, ## args); } while(0)
    #else
        #include <FastSerial.h>
        #define Debug(fmt, args ...)  do {Serial.printf("AP_GPS_MTK16::%s:%d: " fmt "\n", __FUNCTION__, __LINE__, ## args); delay(0); } while(0)
    #endif
#else
 #define Debug(fmt, args ...)
#endif

#if defined(ARDUINO)
    #if ARDUINO >= 100
        #include "Arduino.h"
    #else
        #include <wiring.h>
    #endif
#endif

// Constructors ////////////////////////////////////////////////////////////////
AP_GPS_MTK16::AP_GPS_MTK16(Stream *s) : GPS(s)
{
}

// Public Methods //////////////////////////////////////////////////////////////
void
AP_GPS_MTK16::init(enum GPS_Engine_Setting nav_setting)
{
    _port->flush();

    // initialize serial port for binary protocol use
    // XXX should assume binary, let GPS_AUTO handle dynamic config?
    _port->print(MTK_SET_BINARY);

    // set 5Hz update rate
    _port->print(MTK_OUTPUT_5HZ);

    // set SBAS on
    _port->print(SBAS_ON);

    // set WAAS on
    _port->print(WAAS_ON);

    // set initial epoch code
    _epoch = TIME_OF_DAY;
    _time_offset = 0;
    _offset_calculated = false;
    idleTimeout = 1200;
}

// Process bytes available from the stream
//
// The stream is assumed to contain only our custom message.  If it
// contains other messages, and those messages contain the preamble bytes,
// it is possible for this code to become de-synchronised.  Without
// buffering the entire message and re-processing it from the top,
// this is unavoidable.
//
// The lack of a standard header length field makes it impossible to skip
// unrecognised messages.
//

bool
AP_GPS_MTK16::read(void)
{
    uint8_t data;
    int16_t numc;
    bool parsed = false;
    uint8_t uint32_byte_order[4] = {0, 1, 2, 3}; // big endian
    uint8_t uint16_byte_order[2] = {0, 1}; // big endian
//    uint8_t uint32_byte_order[4] = {3, 2, 1, 0}; // little endian

    if(little_endian_hw())
    {
        uint8_t jj = 3;
        for(int8_t ii=0; ii <= 3; ii++)
        {
           uint32_byte_order[ii] = jj--;
        }
        jj = 1;
        for(int8_t ii=0; ii <= 1; ii++)
        {
           uint16_byte_order[ii] = jj--;
        }

    }

    numc = _port->available();
    Debug("AP_GPS_MTK16.read: numc: %d", (int)numc);

    for (int16_t i = 0; i < numc; i++) {        // Process bytes received

        // read the next byte
        data = _port->read();

restart:
        switch(_step) {

        // Message preamble, class, ID detection
        //
        // If we fail to match any of the expected bytes, we
        // reset the state machine and re-consider the failed
        // byte as the first byte of the preamble.  This
        // improves our chances of recovering from a mismatch
        // and makes it less likely that we will be fooled by
        // the preamble appearing as data in some other message.
        //
        case 0:
            if(PREAMBLE1 == data)
            {
                _step++;
                Debug("AP_GPS_MTK16.read: next step: %d", (int)_step);
            }
            break;
        case 1:
            if (PREAMBLE2 == data)
            {
                _step++;
                Debug("AP_GPS_MTK16.read: next step: %d", (int)_step);
                break;
           }
            Debug("AP_GPS_MTK16.read: step: %d failed!", (int)_step);
            _step = 0;
            goto restart;
        case 2:
            if (0x20 == data)
            {
                _step++;
                _ck_b = _ck_a = data;   // reset the checksum accumulators
                _payload_counter = 0;
                 Debug("AP_GPS_MTK16.read: next step: %d", (int)_step);
            }
            else
            {
                Debug("AP_GPS_MTK16.read: step: %d failed!", (int)_step);
               _step = 0;  // reset and wait for a message of the right class
                goto restart;
            }
            break;

        // Receive message data
        //
        case 3:
            _buffer.bytes[_payload_counter++] = data; // copies byte into byte array
            _ck_b += (_ck_a += data);
            if (_payload_counter == 32) // hex: 0x20 = 32 bytes = message complete
            {
                _step++;
                 Debug("AP_GPS_MTK16.read: next step: %d", (int)_step);
            }
            break;

        // Checksum and message processing
        //
        case 4:
            _step++;
            if (_ck_a != data)
            {
                 Debug("AP_GPS_MTK16.read: step: %d failed!", (int)_step);
                _step = 0;
            }
            break;
        case 5:
            _step = 0;

            if (_ck_b != data)
            {
                Debug("AP_GPS_MTK16.read: no data!!!");
                break;
            }
            // remark: bytes are accessable via .bytes[*] as well as via .msg.XY <- union!!!

            _payload_counter = 0;

            //latitude            = _buffer.msg.latitude  * 10;   // XXX doc says *10e7 but device says otherwise
            latitude =    10*(_buffer.bytes[uint32_byte_order[0] + _payload_counter]<<24 |
                              _buffer.bytes[uint32_byte_order[1] + _payload_counter]<<16 |
                              _buffer.bytes[uint32_byte_order[2] + _payload_counter]<<8  |
                              _buffer.bytes[uint32_byte_order[3] + _payload_counter]
                             );
            _payload_counter += 4;
            //longitude           = _buffer.msg.longitude * 10;   // XXX doc says *10e7 but device says otherwise
            longitude =   10*(_buffer.bytes[uint32_byte_order[0] + _payload_counter]<<24 |
                              _buffer.bytes[uint32_byte_order[1] + _payload_counter]<<16 |
                              _buffer.bytes[uint32_byte_order[2] + _payload_counter]<<8  |
                              _buffer.bytes[uint32_byte_order[3] + _payload_counter]
                             );
            _payload_counter += 4;
            //altitude            = _buffer.msg.altitude;
            altitude =       (_buffer.bytes[uint32_byte_order[0] + _payload_counter]<<24 |
                              _buffer.bytes[uint32_byte_order[1] + _payload_counter]<<16 |
                              _buffer.bytes[uint32_byte_order[2] + _payload_counter]<<8  |
                              _buffer.bytes[uint32_byte_order[3] + _payload_counter]
                             );
            _payload_counter += 4;
            //ground_speed        = _buffer.msg.ground_speed;
            ground_speed =   (_buffer.bytes[uint32_byte_order[0] + _payload_counter]<<24 |
                              _buffer.bytes[uint32_byte_order[1] + _payload_counter]<<16 |
                              _buffer.bytes[uint32_byte_order[2] + _payload_counter]<<8  |
                              _buffer.bytes[uint32_byte_order[3] + _payload_counter]
                             );
            _payload_counter += 4;
            //ground_course       = _buffer.msg.ground_course;
            ground_course =  (_buffer.bytes[uint32_byte_order[0] + _payload_counter]<<24 |
                              _buffer.bytes[uint32_byte_order[1] + _payload_counter]<<16 |
                              _buffer.bytes[uint32_byte_order[2] + _payload_counter]<<8  |
                              _buffer.bytes[uint32_byte_order[3] + _payload_counter]
                             );
            _payload_counter += 4;
            //num_sats      = _buffer.msg.satellites;
            num_sats      = _buffer.bytes[_payload_counter];
            _payload_counter += 1;

            fix           = ((_buffer.msg.fix_type == FIX_3D) ||
                             (_buffer.msg.fix_type == FIX_3D_SBAS));
            _payload_counter += 1;
            //date          = _buffer.msg.utc_date;
            date =           (_buffer.bytes[uint32_byte_order[0] + _payload_counter]<<24 |
                              _buffer.bytes[uint32_byte_order[1] + _payload_counter]<<16 |
                              _buffer.bytes[uint32_byte_order[2] + _payload_counter]<<8  |
                              _buffer.bytes[uint32_byte_order[3] + _payload_counter]
                             );
            _payload_counter += 4;
            // time from gps is UTC, but convert here to msToD
            //int32_t time_utc    = _buffer.msg.utc_time;
            int32_t time_utc=(_buffer.bytes[uint32_byte_order[0] + _payload_counter]<<24 |
                              _buffer.bytes[uint32_byte_order[1] + _payload_counter]<<16 |
                              _buffer.bytes[uint32_byte_order[2] + _payload_counter]<<8  |
                              _buffer.bytes[uint32_byte_order[3] + _payload_counter]
                             );
            _payload_counter += 4;
            int32_t temp = (time_utc/10000000);
            time_utc -= temp*10000000;
            time = temp * 3600000;
            temp = (time_utc/100000);
            time_utc -= temp*100000;
            time += temp * 60000 + time_utc;

            //hdop                        = _buffer.msg.hdop;
            hdop =           (_buffer.bytes[uint16_byte_order[0] + _payload_counter]<<8  |
                              _buffer.bytes[uint16_byte_order[1] + _payload_counter]
                             );
            //_payload_counter += 2;
            parsed = true;

#ifdef FAKE_GPS_LOCK_TIME
            if (millis() > FAKE_GPS_LOCK_TIME*1000) {
                fix                             = true;
                latitude                = -35000000UL;
                longitude               = 149000000UL;
                altitude                = 584;
            }
#endif

            /*	Waiting on clarification of MAVLink protocol!
             *  if(!_offset_calculated && parsed) {
             *                   int32_t tempd1 = date;
             *                   int32_t day    = tempd1/10000;
             *   tempd1         -= day * 10000;
             *                   int32_t month	= tempd1/100;
             *                   int32_t year	= tempd1 - month * 100;
             *   _time_offset = _calc_epoch_offset(day, month, year);
             *   _epoch = UNIX_EPOCH;
             *   _offset_calculated = TRUE;
             *  }
             */

        }
    }
    return parsed;
}


/*
  detect a MTK16 GPS
 */
bool
AP_GPS_MTK16::_detect(uint8_t data)
{
	static uint8_t payload_counter;
	static uint8_t step;
	static uint8_t ck_a, ck_b;

	switch (step) {
        case 1:
            if (PREAMBLE2 == data) {
                step++;
                break;
            }
            step = 0;
        case 0:
			ck_b = ck_a = payload_counter = 0;
            if (PREAMBLE1 == data)
                step++;
			break;
        case 2:
            if (0x20 == data) {
                step++;
                ck_b = ck_a = data;
            } else {
                step = 0;
            }
            break;
        case 3:
            ck_b += (ck_a += data);
            if (++payload_counter == 36)
                step++;
            break;
        case 4:
            step++;
            if (ck_a != data) {
				//Serial.printf("wrong ck_a\n")
                Debug("wrong ck_a in AP_GPS_MTK16::_detect()");
                step = 0;
            }
            break;
        case 5:
            step = 0;
            if (ck_b == data) {
				return true;
            }
            Debug("wrong ck_b in AP_GPS_MTK16::_detect()");
			break;
	}
    return false;
}
