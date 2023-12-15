#include <iostream>
#include <boost/asio.hpp>
#include "fileHandler.hpp"

// paths
// /home/idan/Desktop/CLION_projects/files/f1
// /home/idan/Desktop/CLION_projects/files/dir1

void directory_file_scanner(boost::asio::ip::udp::socket& socket, std::string path, unsigned long chunkSize, std::string baseName);

int main()
{
    try {
        std::cout << "CLIENT IS RUNNING ..." << std::endl;
        boost::asio::io_context io_context;
        // creating a UDP socket
        boost::asio::ip::udp::socket socket(io_context);
        // opening a UDP socket and configuring it to use IPv4 addressing
        socket.open(boost::asio::ip::udp::v4());

        for(;;)
        {
            // get and check the path from client
            std::string path = pathHandler();

            unsigned long chunkSize;
            std::cout << "Please enter the size of each chunk you want to send:" << std::endl;
            std::cin >> chunkSize;

            directory_file_scanner(socket, path, chunkSize, "");
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}

void directory_file_scanner(boost::asio::ip::udp::socket& socket, std::string path, unsigned long chunkSize, std::string baseName)
{
    if (std::filesystem::is_directory(path)) {
        std::string currentDir =  baseName + std::filesystem::path(path).filename().string(); // directory name
        createAndSendConfigPacket(socket, "127.0.0.1", 12345, path,currentDir, chunkSize);
        for (const auto &entry: std::filesystem::directory_iterator(path)) {
            if (std::filesystem::is_regular_file(entry))
            {
                std::string currentPath = entry.path().string();
                std::string currentFileName = currentDir + '/' + entry.path().filename().string();
                createAndSendConfigPacket(socket, "127.0.0.1", 12345, currentPath, currentFileName, chunkSize); // configPacket for each file
                fileSender(socket, "127.0.0.1", 12345, currentPath, chunkSize);
            }
            else // if directory
            {
                std::string base = currentDir + '/' ;
                std::string newPath = entry.path().string();
                directory_file_scanner(socket, newPath, chunkSize, base);
            }
        }
    }
    else // sending a single file (not a directory)
    {
        std::string fileName = std::filesystem::path(path).filename().string(); // directory name
        createAndSendConfigPacket(socket, "127.0.0.1", 12345, path, fileName, chunkSize); // configPacket first
        fileSender(socket, "127.0.0.1", 12345, path, chunkSize);
        std::cout << "SENDING DATA DONE." << std::endl;
    }
}
