#include <iostream>

#include "c_flightgear_sender_asio.hpp"

namespace asio = boost::asio;

c_flightgear_sender_asio::c_flightgear_sender_asio(std::string sReceiverIP, unsigned int iReceiverPort)
    :socket(io_service)
{

    socket.open(asio::ip::udp::v4());

    remote_endpoint = asio::ip::udp::endpoint(
                        asio::ip::address::from_string(sReceiverIP),  iReceiverPort);

    std::cout << "Sending " << sizeof(FGNetFDM)
              << " Bytes to: " << remote_endpoint.address()
              << " on Port: " << remote_endpoint.port()
              << std::endl;

}

c_flightgear_sender_asio::~c_flightgear_sender_asio()
{
    //dtor
}

void c_flightgear_sender_asio::update(void)
{

        boost::system::error_code ignored_error;

        // Send to FlightGear
        socket.send_to(asio::buffer(get_FGData(), sizeof(FGNetFDM)), remote_endpoint, 0, ignored_error);
}
