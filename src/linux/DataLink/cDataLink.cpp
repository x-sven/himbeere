#include "cDataLink.h"

#include <boost/date_time/posix_time/posix_time.hpp>

cDataLink::cDataLink(std::string ip, long port)
{
    sock = 0;
    execute_heartbeat_thread = false;
    this->connect(ip, port);

}

void cDataLink::connect(std::string ip, long port)
{
    gcs_ip = ip;
    gcs_port = port;

    disconnect();

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in locAddr;

    memset(&locAddr, 0, sizeof(locAddr));
    locAddr.sin_family = AF_INET;
    locAddr.sin_addr.s_addr = INADDR_ANY;
    locAddr.sin_port = htons(gcs_port);

    /* Bind the socket to port - necessary to receive packets from qgroundcontrol */
    if (-1 == bind(sock,(struct sockaddr *)&locAddr, sizeof(struct sockaddr)))
    {
        perror("error bind failed");
        close(sock);
        //exit(EXIT_FAILURE);
    }

    /* Attempt to make it non blocking */
    if (fcntl(sock, F_SETFL, O_NONBLOCK | FASYNC) < 0)
    {
        fprintf(stderr, "error setting nonblocking: %s\n", strerror(errno));
        close(sock);
        //exit(EXIT_FAILURE);
    }


    memset(&gcAddr, 0, sizeof(gcAddr));
    gcAddr.sin_family = AF_INET;
    gcAddr.sin_addr.s_addr = inet_addr(gcs_ip.c_str());
    gcAddr.sin_port = htons(14550);

    execute_heartbeat_thread = true;
    heartbeat_thread = boost::thread( boost::bind(&cDataLink::heartbeat_loop, this));
}

void cDataLink::heartbeat_loop(void)
{
    int bytes_sent;
    uint16_t len;

    while(execute_heartbeat_thread)
    {
        /*Send Heartbeat */
        mavlink_msg_heartbeat_pack(1, MAV_COMP_ID_SYSTEM_CONTROL, &msg,
                                   MAV_TYPE_TRICOPTER,
                                   MAV_AUTOPILOT_GENERIC,
                                   MAV_MODE_GUIDED_ARMED,
                                   0, //custom mode bitfield
                                   MAV_STATE_ACTIVE);
        len = mavlink_msg_to_send_buffer(buf, &msg);
        bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&gcAddr, sizeof(struct sockaddr_in));
        (void)bytes_sent; //avoid compiler warning
        boost::this_thread::sleep(boost::posix_time::time_duration(boost::posix_time::milliseconds(500)));
    }
}

void cDataLink::StatusMsg(int load, int millivoltage)
{
    int bytes_sent;
    uint16_t len;

    /* Send Status */
    mavlink_msg_sys_status_pack(1, MAV_COMP_ID_ALL, &msg,
                                0, //sensors present (uint32_bitmask)
                                0, //sensors enabled
                                0, //sensors health
                                load,  //main loop load
                                millivoltage,    //voltage in millivolt
                                -1,    // current in mA (-1 = no data)
                                -1,    // remainig capacity
                                0, 0, 0, 0, 0, 0); //communication and autopilot errors
    len = mavlink_msg_to_send_buffer(buf, &msg);
    bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&gcAddr, sizeof (struct sockaddr_in));
    (void)bytes_sent; //avoid compiler warning

}
void cDataLink::ImuMsg(float ax, float ay, float az,
                       float rx, float ry, float rz,
                       float mx, float my, float mz,
                       float p, float T, float p_alt,
                       long update_mask)
{
    int bytes_sent;
    uint16_t len;

    /* Send high res imu */
    mavlink_msg_highres_imu_pack(1, MAV_COMP_ID_IMU, &msg, microsSinceEpoch(),
                                 ax, ay, az,    //ax, ay, az
                                 rx, ry, rz,    //rx, ry, rz
                                 mx, my, mz,    // mx, my, mz
                                 p,             //abs
                                 0.,            //diff
                                 p_alt,         //pressure_alt
                                 T,             //temperature
                                 update_mask);  //bitmask of sensor updates
    len = mavlink_msg_to_send_buffer(buf, &msg);
    bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&gcAddr, sizeof(struct sockaddr_in));
    (void)bytes_sent; //avoid compiler warning
}

void cDataLink::GpsMsg(unsigned short fix_type, long lat, long lon, long alt,
                       long vdop, long hdop,
                       long vel, long course, unsigned short sats)
{
    int bytes_sent;
    uint16_t len;

    /* Send GPS raw int */
    mavlink_msg_gps_raw_int_pack(1, MAV_COMP_ID_GPS, &msg, microsSinceEpoch(),
                                 fix_type,          //fix_type
                                 lat, lon, alt,  //lat, lon, alt
                                 vdop, hdop,  // hdop, vdop
                                 vel,      //vel*100
                                 course,       //course deg*100
                                 sats);         //visible satellites
    len = mavlink_msg_to_send_buffer(buf, &msg);
    bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&gcAddr, sizeof(struct sockaddr_in));
    (void)bytes_sent; //avoid compiler warning
}

void cDataLink::AttMsg(float roll, float pitch, float yaw,
                       float rollspeed, float pitchspeed, float yawspeed)
{
    int bytes_sent;
    uint16_t len;

    /* Send attitude */
    mavlink_msg_attitude_pack(1, MAV_COMP_ID_ALL, &msg, microsSinceEpoch(),
                              roll, pitch, yaw,
                              rollspeed, pitchspeed, yawspeed);
    len = mavlink_msg_to_send_buffer(buf, &msg);
    bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&gcAddr, sizeof(struct sockaddr_in));
    (void)bytes_sent; //avoid compiler warning
}

void cDataLink::disconnect(void)
{
    execute_heartbeat_thread = false;

    //...close socket etc.
    close(sock);

}

cDataLink::~cDataLink()
{
    //dtor
    disconnect();
}

/* QNX timer version */
#if (defined __QNX__) | (defined __QNXNTO__)
uint64_t cDataLink::microsSinceEpoch()
{

    struct timespec time;

    uint64_t micros = 0;

    clock_gettime(CLOCK_REALTIME, &time);
    micros = (uint64_t)time.tv_sec * 100000 + time.tv_nsec/1000;

    return micros;
}
#else
uint64_t cDataLink::microsSinceEpoch()
{

    struct timeval tv;

    uint64_t micros = 0;

    gettimeofday(&tv, NULL);
    micros =  ((uint64_t)tv.tv_sec) * 1000000 + tv.tv_usec;

    return micros;
}
#endif
