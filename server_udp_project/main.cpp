#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <boost/asio.hpp>
#include "../fileStructure.pb.h"

void receivePacket(boost::asio::ip::udp::socket& socket, unsigned long packetsNumber, unsigned int chunkSize, std::string& finalPath);
void createFileAndWrite(std::vector<std::string>& contentVector, unsigned long packetsNumber, std::string& finalPath);

int main() {
    try {
        std::cout << "SERVER IS RUNNING..." << std::endl;
        boost::asio::io_context io_context;

        std::vector<std::string> packetsVector(10);

        // creates a UDP socket binding it to the local UDP port 12345 using IPv4.
        boost::asio::ip::udp::socket socket(
                io_context,
                boost::asio::ip::udp::endpoint{boost::asio::ip::udp::v4(), 12345});

        boost::asio::ip::udp::endpoint client;

        std::string basePath = "/home/idan/Desktop/CLION_projects/UDP_NETWORKING/server_udp_project/files_from_client/";

        for (;;) {
            try {
                FILE_STORAGE::ConfigPacket configPacket;
                char receive_data[300]; // The max possible size
                std::size_t bytesRead = socket.receive_from(boost::asio::buffer(receive_data), client);
                std::cout << "CONFIGURATION PACKET -> READS " << bytesRead << " BYTES." << std::endl;
                configPacket.ParseFromString(std::string(receive_data, bytesRead));
                if (configPacket.type() == FILE_STORAGE::FileType::DIRECTORY)
                    std::filesystem::create_directory(basePath + configPacket.name());
                else {
                    std::string filePath = basePath + configPacket.name();
                    receivePacket(socket, configPacket.chunks(), configPacket.chunk_size(), filePath);
                }
            } catch (std::exception& e) {
                std::cerr << "Error occurred!\n" << e.what() << std::endl; // Unexpected packet
            }
        }
    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

void receivePacket(boost::asio::ip::udp::socket& socket, unsigned long packetsNumber, unsigned int chunkSize, std::string& finalPath) {
    std::vector<std::string> packetsVector(packetsNumber);
    FILE_STORAGE::Packet packet; // creating a packet object
    char receive_data[chunkSize + 32]; // getting the data from socket
    boost::asio::ip::udp::endpoint client;
    boost::system::error_code error;
    for (unsigned long i = 0; i < packetsNumber; ++i)
    {
        std::size_t bytesRead = socket.receive_from(boost::asio::buffer(receive_data, sizeof(receive_data)), client, 0, error);
        if (error)
            std::cerr << "Error receiving data: " << error.message() << std::endl;
        std::cout << "Received " << bytesRead << " bytes from the client." << std::endl;
        std::cout << "NEW PACKET FROM CLIENT CONNECTION: " << client << std::endl;
        packet.ParseFromString(std::string(receive_data, bytesRead)); // parse back all the bytes that read
        std::cout << "PACKET RECEIVE SIZE: " << packet.ByteSizeLong() << "." << std::endl;
        std::cout << "PACKET FILE CONTENT RECEIVE SIDE: \n" << packet.file_content() << "." << std::endl;
        packetsVector[packet.id() - 1] = packet.file_content();
        memset(receive_data, 0, sizeof(receive_data));
    }
    createFileAndWrite(packetsVector, packetsNumber, finalPath);
}

void createFileAndWrite(std::vector<std::string>& contentVector, unsigned long packetsNumber, std::string& finalPath)
{
    std::string finalFileContent;
    std::ofstream file(finalPath); // opening the file in writing mode
    unsigned int lostPackets = 0;
    for (unsigned long i = 0; i < packetsNumber; ++i)
    {
        if (contentVector[i].empty())
        {
            std::cout << "Packet number: " << i+1 << " get lost..." << std::endl;
            lostPackets += 1;
        }
        // delete the nulls at the end from the first/last packet
        else if (i == packetsNumber - 1)
                contentVector[i].erase(std::find(contentVector[i].begin(),contentVector[i].end(), '\0'),
                                       contentVector[i].end());

        finalFileContent += contentVector[i];
        if (finalFileContent.length() > 512)
        {
            std::cout << "get some space in the vector..." << std::endl;
            file << finalFileContent;
            finalFileContent = "";
        }
    }
    file << finalFileContent;
    file.close();
    std::cout << "The number of lost packets is: " << lostPackets << " packets." << std::endl;
}
