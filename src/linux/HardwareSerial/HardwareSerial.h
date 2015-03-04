#ifndef HEADER_6AC8F54DC40F1976
#define HEADER_6AC8F54DC40F1976

/*
  HardwareSerial.h - Hardware serial library for Wiring
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

  Modified 28 September 2010 by Mark Sproul
*/

#include <inttypes.h>
#include <termios.h>

#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>

#include "ARDUINO/Stream.h"


#define SERIAL_BUFFER_SIZE 256
struct ring_buffer
{
  unsigned char buffer[SERIAL_BUFFER_SIZE];
  volatile int head;
  volatile int tail;
};

class HardwareSerial : public Stream
{
  private:
    ring_buffer rx_buffer;
    ring_buffer tx_buffer;
    ring_buffer* _rx_buffer;
    ring_buffer* _tx_buffer;

    int fd; //! file descriptor
    termios ios_config_backup;       //!< the settings of the port before modified here.

    boost::thread receiving_thread;
    bool thread_running;
    void receive_loop(void);

  public:
    HardwareSerial( const char* device);
    virtual ~HardwareSerial(){};
    void begin(unsigned long);
    void end();
    virtual int available(void);
    virtual int read(void);
    virtual int peek(void);
    virtual void flush(void);
    virtual size_t write(uint8_t);
    using Print::write; // pull in write(str) and write(buf, size) from Print

    boost::signals2::signal<void (void)>  signal_newdata;

};

#endif // header guard

