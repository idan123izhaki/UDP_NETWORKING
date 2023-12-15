#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <unistd.h>
#include <boost/asio.hpp>
#include "../fileStructure.pb.h"

#ifndef CLIENT_UDP_PROJECT_FILEHANDLER_HPP
#define CLIENT_UDP_PROJECT_FILEHANDLER_HPP

std::string pathHandler();

void createAndSendConfigPacket(boost::asio::ip::udp::socket& socket, const std::string& ipAddress, unsigned short port, std::string& path, std::string& currentName, unsigned long chunkSize);

void fileSender(boost::asio::ip::udp::socket& socket, const std::string& ipAddress, unsigned short port, std::string& path, unsigned long chunkSize);

#endif //CLIENT_UDP_PROJECT_FILEHANDLER_HPP
