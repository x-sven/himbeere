#include "cDataLink.h"
#include "cParameter/cParameter.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <bitset>

#define MAVLINK_MSG_ID_PARAM_REQUEST_READ 20

enum control_sensors
{
    gyro,                       //0: 3D
    acc,                        //1: 3D
    mag,                        //2: 3D
    absolute_pressure,          //3:
    differential_pressure,      //4:
    gps,                        //5:
    optical_flow,               //6:
    computer_vision_position,   //7:
    laser_based_position,       //8:
    external_ground_truth,      //9:
    angular_rate_control,       //10:
    attitude_stabilization,     //11:
    yaw_position,               //12:
    z_altitude_control,         //13:
    x_y_position_control,       //14:
    motor_outputs_control       //15:
};

cDataLink::cDataLink(std::string ip, long port)
{
    sock = 0;
    packet_drops = 0;
    mav_state = MAV_STATE_UNINIT;
    mav_mode = MAV_MODE_PREFLIGHT;
    execute_heartbeat_thread = false;
    execute_receive_thread = false;

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

    execute_receive_thread = true;
    receive_thread = boost::thread( boost::bind(&cDataLink::receive_loop, this));
}

void cDataLink::heartbeat_loop(void)
{
    int bytes_sent;
    uint16_t len;

    while(execute_heartbeat_thread)
    {
        /*Send Heartbeat */
        //MAV_TYPE_TRICOPTER
        mavlink_msg_heartbeat_pack(1, MAV_COMP_ID_SYSTEM_CONTROL, &m_msg,
                                   MAV_TYPE_TRICOPTER,
                                   MAV_AUTOPILOT_GENERIC,
                                   mav_mode,
                                   0, //custom mode bitfield
                                   mav_state);
        len = mavlink_msg_to_send_buffer(m_buf, &m_msg);
        bytes_sent = sendto(sock, m_buf, len, 0, (struct sockaddr*)&gcAddr, sizeof(struct sockaddr_in));
        (void)bytes_sent; //avoid compiler warning
        boost::this_thread::sleep(boost::posix_time::time_duration(boost::posix_time::milliseconds(500)));
    }
}

void cDataLink::receive_loop(void)
{
    const uint16_t BUFFER_LENGTH = 2048;

    uint8_t buf[BUFFER_LENGTH];
    ssize_t recsize;
    socklen_t fromlen;

    memset(buf, 0, BUFFER_LENGTH);

    while(execute_receive_thread)
    {
        recsize = recvfrom(sock, (void *)buf, BUFFER_LENGTH, 0, (struct sockaddr *)&gcAddr, &fromlen);
        if (recsize > 0)
        {
            // Something received - print out all bytes and parse packet
            mavlink_message_t msg;
            mavlink_status_t status;

            for (ssize_t i = 0; i < recsize; ++i)
            {
                if (mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status))
                {
                    // Packet received
                    handleMessage(&msg);
                }// parse is ok
            }// for all bytes received
        }
        memset(buf, 0, BUFFER_LENGTH);
        boost::this_thread::sleep(boost::posix_time::time_duration(boost::posix_time::milliseconds(10)));
    }//while
}

void cDataLink::handleMessage(mavlink_message_t* msg)
{

    uint16_t len;
    int bytes_sent;

    mavlink_param_set_t set;
    mavlink_msg_param_set_decode(msg, &set);

    //uint8_t result = MAV_RESULT_UNSUPPORTED;
    switch (msg->msgid)
    {
    case MAVLINK_MSG_ID_COMMAND_LONG:
    {
        // decode
        mavlink_command_long_t packet;
        mavlink_msg_command_long_decode(msg, &packet);

        switch(packet.command)
        {
        case MAV_CMD_PREFLIGHT_CALIBRATION:
            // 1: Gyro calibration
            if (packet.param2 == 1)
            {
                printf("Magnetometer calibration!\n");
                signal_mag_calibration();
            }
            // 3: Ground pressure
            // 4: Radio calibration
            // 5: Accelerometer calibration
            // Parameter 6 & 7 not pre-defined
            //                result = MAV_RESULT_ACCEPTED;
            break;
        default:
            //                result = MAV_RESULT_UNSUPPORTED;
            break;
        } // switch packet.command
//            mavlink_msg_command_ack_send(chan,
//                                         packet.command,
//                                         result);
        break;
    } // case MAVLINK_MSG_ID_COMMAND_LONG
    case MAVLINK_MSG_ID_PARAM_REQUEST_READ:
    {
        mavlink_param_request_read_t packet;
        mavlink_msg_param_request_read_decode(msg, &packet);

        mavlink_msg_param_value_pack(
            packet.target_system,
            packet.target_component,
            msg,
            cParameter::get_instances()->at(packet.param_index)->get_name().c_str(),
            *(cParameter::get_instances()->at(packet.param_index)->get_value()),
            MAV_PARAM_TYPE_REAL32,
            cParameter::get_instances()->size(),      // param_count = Anzahl der Parameter
            packet.param_index);                      // param_index = Nummer f. aktellen Parameter
        len = mavlink_msg_to_send_buffer(m_buf, msg);
        bytes_sent = sendto(sock, m_buf, len, 0, (struct sockaddr*)&gcAddr, sizeof (struct sockaddr_in));
        (void)bytes_sent; //avoid compiler warning


        break;
    }
    case MAVLINK_MSG_ID_PARAM_REQUEST_LIST:
    {
        // Start sending parameters
        for(std::vector<cParameter*>::iterator it=cParameter::get_instances()->begin(); it!=cParameter::get_instances()->end(); ++it)
        {
            mavlink_msg_param_value_pack(
                1, MAV_COMP_ID_SYSTEM_CONTROL,
                msg,
                (*it)->get_name().c_str(),
                *((*it)->get_value()),   //Iterator goes to vector list with cParameter pointers. Get value returns a pointer to the component value -> again dereference
                MAV_PARAM_TYPE_REAL32,
                cParameter::get_instances()->size(),                         // param_count = Anzahl der Parameter
                std::distance(cParameter::get_instances()->begin(),it));     // param_index = Nummer f. aktellen Parameter
            len = mavlink_msg_to_send_buffer(m_buf, msg);
            bytes_sent = sendto(sock, m_buf, len, 0, (struct sockaddr*)&gcAddr, sizeof (struct sockaddr_in));
            (void)bytes_sent; //avoid compiler warning
            boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        }
        break;
    }
    case MAVLINK_MSG_ID_PARAM_SET:
    {
        mavlink_param_set_t packet;
        mavlink_msg_param_set_decode(msg, &packet);

        std::vector<cParameter*>::iterator it = cParameter::exist(packet.param_id);

        if( cParameter::get_instances()->end() != it )
        {
            (*it)->set_value(packet.param_value);

            mavlink_msg_param_value_pack(
                packet.target_system,
                packet.target_component,
                msg,
                (*it)->get_name().c_str(),
                *((*it)->get_value()),
                MAV_PARAM_TYPE_REAL32,
                cParameter::get_instances()->size(),                     // param_count = Anzahl der Parameter
                std::distance(cParameter::get_instances()->begin(),it)); // param_index = Nummer f. aktellen Parameter
            len = mavlink_msg_to_send_buffer(m_buf, msg);
            bytes_sent = sendto(sock, m_buf, len, 0, (struct sockaddr*)&gcAddr, sizeof (struct sockaddr_in));
            (void)bytes_sent; //avoid compiler warning
        }
        break;
    }
    default:
        {
          printf("msg->msgid not known: %d\n", msg->msgid);
        }
        break;
    }// switch msgid

}

void cDataLink::SendStatusMsg(int load, int millivoltage)
{
    int bytes_sent;
    uint16_t len;



    std::bitset<16> onboard_control_sensors_present = 0;
    std::bitset<16> onboard_control_sensors_enabled = 0;
    std::bitset<16> onboard_control_sensors_health = 0;


    onboard_control_sensors_present.set(gyro);
    onboard_control_sensors_present.set(acc);
    onboard_control_sensors_present.set(mag);
    onboard_control_sensors_present.set(absolute_pressure);
    onboard_control_sensors_present.set(gps);

    onboard_control_sensors_enabled = onboard_control_sensors_present;
    onboard_control_sensors_health  = onboard_control_sensors_present;

    /* Send Status */
    mavlink_msg_sys_status_pack(1, MAV_COMP_ID_ALL, &m_msg,
                                onboard_control_sensors_present.to_ulong(), //sensors present (uint32_bitmask)
                                onboard_control_sensors_enabled.to_ulong(), //sensors enabled
                                onboard_control_sensors_health.to_ulong(),  //sensors health
                                load,  //main loop load
                                millivoltage,    //voltage in millivolt
                                -1,    // current in mA (-1 = no data)
                                -1,    // remainig capacity
                                0, 0, 0, 0, 0, 0); //communication and autopilot errors
    len = mavlink_msg_to_send_buffer(m_buf, &m_msg);
    bytes_sent = sendto(sock, m_buf, len, 0, (struct sockaddr*)&gcAddr, sizeof (struct sockaddr_in));
    (void)bytes_sent; //avoid compiler warning

}

//static float counter =0;

void cDataLink::SendImuMsg(float ax, float ay, float az,
                           float rx, float ry, float rz,
                           float mx, float my, float mz,
                           float p, float T, float p_alt,
                           long update_mask)
{
    int bytes_sent;
    uint16_t len;

//    ax = 1;
//    ay = 2;
//    az = 9.81;
//
//
//
//    counter = counter + 3.14*0.01;
//
//     mx=.1*cos(counter)*cos(counter)+0.1;
//     my=.1*sin(counter)*cos(counter)+0.2;
//     mz=0.1*sin(counter);
//
//    printf("mag: %f, %f, %f\n", mx, my, mz);

    /* Send high res imu */
    mavlink_msg_highres_imu_pack(1, MAV_COMP_ID_IMU, &m_msg, microsSinceEpoch(),
                                 ax, ay, az,    //ax, ay, az
                                 rx, ry, rz,    //rx, ry, rz
                                 mx, my, mz,    // mx, my, mz
                                 p,             //abs
                                 0.,            //diff
                                 p_alt,         //pressure_alt
                                 T,             //temperature
                                 update_mask);  //bitmask of sensor updates

    len = mavlink_msg_to_send_buffer(m_buf, &m_msg);
    bytes_sent = sendto(sock, m_buf, len, 0, (struct sockaddr*)&gcAddr, sizeof(struct sockaddr_in));

    /* Send raw_imu */
    mavlink_msg_raw_imu_pack(1, MAV_COMP_ID_IMU, &m_msg, microsSinceEpoch(),
                                 (int16_t)(ax*100.), (int16_t)(ay*100.), (int16_t)(az*100.),    //ax, ay, az
                                 (int16_t)(rx*100.), (int16_t)(ry*100.), (int16_t)(rz*100.),    //rx, ry, rz
                                 (int16_t)(mx*1000.), (int16_t)(my*1000.), (int16_t)(mz*1000.));  //bitmask of sensor updates

    len = mavlink_msg_to_send_buffer(m_buf, &m_msg);
    bytes_sent = sendto(sock, m_buf, len, 0, (struct sockaddr*)&gcAddr, sizeof(struct sockaddr_in));

    /* Send high scaled_imu */
    mavlink_msg_scaled_imu_pack(1, MAV_COMP_ID_IMU, &m_msg, microsSinceEpoch(),
                                 (int16_t)(ax*100.), (int16_t)(ay*100.), (int16_t)(az*100.),    //ax, ay, az
                                 (int16_t)(rx*100.), (int16_t)(ry*100.), (int16_t)(rz*100.),    //rx, ry, rz
                                 (int16_t)(mx*1000.), (int16_t)(my*1000.), (int16_t)(mz*1000.));  //bitmask of sensor updates

    len = mavlink_msg_to_send_buffer(m_buf, &m_msg);
    bytes_sent = sendto(sock, m_buf, len, 0, (struct sockaddr*)&gcAddr, sizeof(struct sockaddr_in));

    (void)bytes_sent; //avoid compiler warning
}

void cDataLink::SendGpsMsg(unsigned short fix_type, long lat, long lon, long alt,
                           long vdop, long hdop,
                           long vel, long course, unsigned short sats)
{
    int bytes_sent;
    uint16_t len;

    /* Send GPS raw int */
    mavlink_msg_gps_raw_int_pack(1, MAV_COMP_ID_GPS, &m_msg, microsSinceEpoch(),
                                 fix_type,          //fix_type
                                 lat, lon, alt,  //lat, lon, alt
                                 vdop, hdop,  // hdop, vdop
                                 vel,      //vel*100
                                 course,       //course deg*100
                                 sats);         //visible satellites
    len = mavlink_msg_to_send_buffer(m_buf, &m_msg);
    bytes_sent = sendto(sock, m_buf, len, 0, (struct sockaddr*)&gcAddr, sizeof(struct sockaddr_in));
    (void)bytes_sent; //avoid compiler warning
}

void cDataLink::SendAttMsg(float roll, float pitch, float yaw,
                           float rollspeed, float pitchspeed, float yawspeed)
{
    int bytes_sent;
    uint16_t len;

    /* Send attitude */
    mavlink_msg_attitude_pack(1, MAV_COMP_ID_ALL, &m_msg, microsSinceEpoch(),
                              roll, pitch, yaw,
                              rollspeed, pitchspeed, yawspeed);
    len = mavlink_msg_to_send_buffer(m_buf, &m_msg);
    bytes_sent = sendto(sock, m_buf, len, 0, (struct sockaddr*)&gcAddr, sizeof(struct sockaddr_in));
    (void)bytes_sent; //avoid compiler warning
}

void cDataLink::SendRcChannelsRaw(uint16_t ped_us, uint16_t col_us, uint16_t lon_us,
                                  uint16_t lat_us, uint16_t aux_us,uint16_t mode_us)
{
    int bytes_sent;
    uint16_t len;

    /* Send RC channels */
    {
        mavlink_msg_rc_channels_raw_pack(1, MAV_COMP_ID_ALL, &m_msg, microsSinceEpoch(),
                                         uint8_t(1),                     // port
                                         ped_us, col_us, lon_us, lat_us, //1...4
                                         aux_us, mode_us,                //5, 6
                                         uint16_t(65535), uint16_t(65535), // 6...8, 65535 implies the channel is unused
                                         uint8_t(255));                   // rssi = unknown
        len = mavlink_msg_to_send_buffer(m_buf, &m_msg);
        bytes_sent = sendto(sock, m_buf, len, 0, (struct sockaddr*)&gcAddr, sizeof(struct sockaddr_in));
        (void)bytes_sent; //avoid compiler warning
    }
}

void cDataLink::SendServoOutputRaw(uint16_t ped_us, uint16_t col_us, uint16_t lon_us,
                                   uint16_t lat_us, uint16_t aux_us, uint16_t mode_us)
{
    int bytes_sent;
    uint16_t len;

    /* Send RC channels */
    {
        mavlink_msg_servo_output_raw_pack(1, MAV_COMP_ID_ALL, &m_msg, microsSinceEpoch(),
                                         uint8_t(1),                     // port
                                         ped_us, col_us, lon_us, lat_us, //1...4
                                         aux_us, mode_us,                //5, 6
                                         uint16_t(65535), uint16_t(65535)); // 6...8, 65535 implies the channel is unused
        len = mavlink_msg_to_send_buffer(m_buf, &m_msg);
        bytes_sent = sendto(sock, m_buf, len, 0, (struct sockaddr*)&gcAddr, sizeof(struct sockaddr_in));
        (void)bytes_sent; //avoid compiler warning
    }
}
void cDataLink::disconnect(void)
{
    execute_heartbeat_thread = false;
    execute_receive_thread   = false;

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
