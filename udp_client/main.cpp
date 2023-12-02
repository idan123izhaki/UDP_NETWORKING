#include <boost/asio.hpp>
#include <iostream>

int main()
{
    try
    {
        std::cout << "CLIENT IS RUNNING ..." << std::endl;

        boost::asio::io_context io_context;

        boost::asio::ip::udp::socket socket(io_context);
        socket.open(boost::asio::ip::udp::v4());

        socket.send_to(
                boost::asio::buffer("Hello world!"),
                boost::asio::ip::udp::endpoint{boost::asio::ip::make_address("127.0.0.1"), 12345}
                );
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}
