#include <iostream>
#include <ctime>
#include <functional>
#include <string>
#include <boost/asio.hpp>

int main() {
    try
    {
        std::cout << "SERVER IS RUNNING ..." << std::endl;
        boost::asio::io_context io_context;

        for(;;)
        {
            boost::asio::ip::udp::socket socket(
                    io_context,
                    boost::asio::ip::udp::endpoint{boost::asio::ip::udp::v4(), 12345});

            boost::asio::ip::udp::endpoint client;
            char recv_str[1024] = {};

            socket.receive_from(
                    boost::asio::buffer(recv_str),
                    client);

            std::cout << client << ": " << recv_str << std::endl;
        }
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
