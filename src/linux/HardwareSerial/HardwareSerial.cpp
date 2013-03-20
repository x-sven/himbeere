/*
  HardwareSerial.cpp - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 23 November 2006 by David A. Mellis
  Modified 28 September 2010 by Mark Sproul
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "HardwareSerial.h"

#include <fcntl.h> /* open, write, read... & file status flags and file access modes of the open file description*/
#include <unistd.h> /*read - read from a file */
#include <limits.h>
#include <termios.h>

#include <iostream>

inline void store_char(unsigned char c, ring_buffer *buffer)
{
    int i = (unsigned int)(buffer->head + 1) % SERIAL_BUFFER_SIZE;

    // if we should be storing the received character into the location
    // just before the tail (meaning that the head would advance to the
    // current location of the tail), we're about to overflow the buffer
    // and so we don't write the character or advance the head.
    if (i != buffer->tail)
    {
        buffer->buffer[buffer->head] = c;
        buffer->head = i;
    }
}


void HardwareSerial::receive_loop(void)
{
    try
    {
        while(thread_running)
        {
            unsigned char c  =  0;
            ssize_t return_value = 0;

            if(fd != -1)
            {
                /* unistd.h: Read NBYTES into BUF from FD.  Return the
                             number read, -1 for errors or 0 for EOF.*/
                /* It follows a BLOCKING read from fd*/
                while(0 == return_value || SSIZE_MAX < return_value || EOF == return_value)
                {
                    return_value = ::read(fd, &c, 1); //timout is set to X seconds below
                    /* For using boost::thread::interrupt(), we have to use boost::thread::sleep() for it to work. */
                    boost::this_thread::sleep(boost::posix_time::milliseconds(10));
                }
                store_char(c, _rx_buffer);
            }
            else
            {
                std::cout << "serial interface: file descriptor not valid" << std::endl;

            }
            boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        }
        std::cout << "leaving serial receive_loop thread" << std::endl;

    }
    catch(boost::thread_interrupted const& )
    {
        //clean resources
        std::cout << "receive_loop thread interrupted" << std::endl;
    }

}



// Constructors ////////////////////////////////////////////////////////////////

HardwareSerial::HardwareSerial( const char* device)
{
    _rx_buffer = &rx_buffer;
    _tx_buffer = &tx_buffer;

    fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
#if defined(DEBUG)
    if(fd == -1)
    {
        std::cout << "Could not open device " << device
                  << " in HardwareSerial::HardwareSerial()!"
                  << std::endl << fflush(stdout);
    }
    thread_running = false;
#endif // defined
}

// Public Methods //////////////////////////////////////////////////////////////
void HardwareSerial::begin(unsigned long baudrate)
{
    if(fd != -1)
    {
        termios ios_config;

        if (tcgetattr(fd, &ios_config)>=0)
        {
            ios_config_backup = ios_config;
            // BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
            switch (baudrate)
            {
            case 0:
                ios_config.c_cflag = B0;
                break;
            case 50:
                ios_config.c_cflag = B50;
                break;
            case 75:
                ios_config.c_cflag = B75;
                break;
            case 110:
                ios_config.c_cflag = B110;
                break;
            case 134:
                ios_config.c_cflag = B134;
                break;
            case 150:
                ios_config.c_cflag = B150;
                break;
            case 200:
                ios_config.c_cflag = B200;
                break;
            case 300:
                ios_config.c_cflag = B300;
                break;
            case 600:
                ios_config.c_cflag = B600;
                break;
            case 1200:
                ios_config.c_cflag = B1200;
                break;
            case 1800:
                ios_config.c_cflag = B1800;
                break;
            case 2400:
                ios_config.c_cflag = B2400;
                break;
            case 4800:
                ios_config.c_cflag = B4800;
                break;
            case 9600:
                ios_config.c_cflag = B9600;
                break;
            case 19200:
                ios_config.c_cflag = B19200;
                break;
            case 38400:
                ios_config.c_cflag = B38400;
                break;
            case 57600:
                ios_config.c_cflag = B57600;
                break;
            case 115200:
                ios_config.c_cflag = B115200;
                break;
            case 230400:
                ios_config.c_cflag = B230400;
                break;
            default:
                ios_config.c_cflag = B9600;
                break;
            }
            // CS8     : 8n1 (8bit,no parity,1 stopbit)
            // CLOCAL  : local connection, no modem contol
            // CREAD   : enable receiving characters
            ios_config.c_cflag = ios_config.c_cflag | CS8 | CLOCAL | CREAD;
            ios_config.c_iflag = (IGNPAR);                 // IGNPAR: Ignore bytes with parity errors
            ios_config.c_oflag = 0;                        // Raw output
            ios_config.c_lflag = 0;                        // Set input mode (non-canonical, no echo,...)
            ios_config.c_cc[VTIME] = 10;                   // Set timeout of 1.0 seconds
            ios_config.c_cc[VMIN]  = 1;                    // Blocking read until "min" chars received
            // info: no flowcontrol used here
            tcflush(fd, TCIFLUSH);                         // Flush terminal
            if( tcsetattr(fd, TCSANOW, &ios_config) >= 0 ) // Apply configuration
            {
                // start thread in sucess
                thread_running = true;
                receiving_thread = boost::thread( boost::bind(&HardwareSerial::receive_loop, this) );
            }
            #if defined(DEBUG)
            else
            {
                std::cout << "Could not apply configuration"
                          << " in HardwareSerial::begin()!"
                          << std::endl << fflush(stdout);
            }
            #endif // DEBUG
        }// if tcgetattr()
        #if defined(DEBUG)
        else
        {
            std::cout << "Could not get tcgetattr()"
                      << " in HardwareSerial::begin()!"
                      << std::endl << fflush(stdout);
        }
        #endif // DEBUG
    }// if fd
    else
    {
        std::cout << "serial interface: file descriptor not valid" << std::endl;
    }
}

void HardwareSerial::end()
{
    #if defined(DEBUG)
    std::cout << "Time to interrupt the serial thread_loop()..." << std::endl;
    #endif
    thread_running = false;

    // wait for transmission of outgoing data
    while (_tx_buffer->head != _tx_buffer->tail)
        ;

    // clear any received data
    _rx_buffer->head = _rx_buffer->tail;

    //receiving_thread.join();

    if(fd != -1)
    {
        tcsetattr(fd, TCSANOW, &ios_config_backup);
        ::close(fd);
        fd = -1;
    }

}

int HardwareSerial::available(void)
{
    return (unsigned int)(SERIAL_BUFFER_SIZE + _rx_buffer->head - _rx_buffer->tail) % SERIAL_BUFFER_SIZE;
}

int HardwareSerial::peek(void)
{
    if (_rx_buffer->head == _rx_buffer->tail)
    {
        return -1;
    }
    else
    {
        return _rx_buffer->buffer[_rx_buffer->tail];
    }
}

int HardwareSerial::read(void)
{
//    int buffer = 0;
//    ssize_t return_value = 0;
//
//    if(fd != -1)
//    {
//        /* unistd.h: Read NBYTES into BUF from FD.  Return the
//                     number read, -1 for errors or 0 for EOF.*/
//        /* It follows a BLOCKING read from fd*/
//        while(0 == return_value || SSIZE_MAX < return_value || EOF == return_value)
//        {
//            return_value = ::read(fd, &buffer, 1);
//        }
//
//    }
//    return (int)buffer;

    // if the head isn't ahead of the tail, we don't have any characters
    if (_rx_buffer->head == _rx_buffer->tail)
    {
        return -1;
    }
    else
    {
        unsigned char c = _rx_buffer->buffer[_rx_buffer->tail];
        _rx_buffer->tail = (unsigned int)(_rx_buffer->tail + 1) % SERIAL_BUFFER_SIZE;
        return c;
    }
}

void HardwareSerial::flush()
{
    while (_tx_buffer->head != _tx_buffer->tail)
        ;
}

size_t HardwareSerial::write(uint8_t c)
{
    return( ::write(fd, (void*)&c, (size_t)1) );
}


