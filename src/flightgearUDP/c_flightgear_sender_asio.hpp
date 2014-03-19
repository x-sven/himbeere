#ifndef C_FLIGHTGEAR_SENDER_ASIO_HPP
#define C_FLIGHTGEAR_SENDER_ASIO_HPP

#include <string>
#include <boost/asio.hpp>

#include "c_flightgear_base.hpp"


class c_flightgear_sender_asio : public c_flightgear_base
{
public:
    c_flightgear_sender_asio(std::string sReceiverIP = "127.0.0.1", unsigned int iReceiverPort = 5510);
    ~c_flightgear_sender_asio();

    void update(void);

protected:
private:

    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint remote_endpoint;
};

#endif // C_FLIGHTGEAR_SENDER_ASIO_HPP
