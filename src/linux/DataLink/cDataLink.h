#ifndef __CDATALINK_H__
#define __CDATALINK_H__

#include <string>
/* These headers are for QNX, but should all be standard on unix/linux */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#if (defined __QNX__) | (defined __QNXNTO__)
/* QNX specific headers */
#include <unix.h>
#else
/* Linux / MacOS POSIX timer headers */
#include <sys/time.h>
#include <time.h>
#include <arpa/inet.h>
#endif

#include <boost/thread.hpp>
#include <boost/signal.hpp>

#include "mavlink/include/common/mavlink.h"


class cDataLink
{
    public:
        cDataLink(std::string ip = "127.0.0.1", long gcs_port=14551);
        void connect(std::string ip = "127.0.0.1", long port=14551);
        void disconnect(void);

        void SendStatusMsg(int load, int millivoltage);
        void SendImuMsg(float ax, float ay, float az,
                    float rx, float ry, float rz,
                    float mx, float my, float mz,
                    float p, float T, float p_alt,
                    long update_mask);
        void SendGpsMsg(unsigned short fix_type, long lat, long lon, long alt,
                        long vdop, long hdop,
                        long vel, long course, unsigned short sats);
        void SendAttMsg(float roll, float pitch, float yaw,
                    float rollspeed, float pitchspeed, float yawspeed);
        void SendRcChannelsRaw(uint16_t ped_us, uint16_t col_us, uint16_t lon_us,
                               uint16_t lat_us, uint16_t aux_us, uint16_t mode_us);
        void SendServoOutputRaw(uint16_t ped_us, uint16_t col_us, uint16_t lon_us,
                                uint16_t lat_us, uint16_t aux_us, uint16_t mode_us);

        boost::signal<void (void)>  signal_mag_calibration;

        virtual ~cDataLink();
    protected:
    private:
        std::string gcs_ip;
        long gcs_port;

        int sock;
        struct sockaddr_in gcAddr;

        uint8_t m_buf[2048];
        mavlink_message_t m_msg;

        boost::thread heartbeat_thread;
        bool execute_heartbeat_thread;

        boost::thread receive_thread;
        bool execute_receive_thread;

        void heartbeat_loop(void);
        void receive_loop(void);

        void handleMessage(mavlink_message_t* msg);

        uint64_t microsSinceEpoch(void);

        int packet_drops;
        int mav_mode;
        int mav_state;


};

#endif // __CDATALINK_H__
