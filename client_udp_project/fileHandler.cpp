#include "fileHandler.hpp"
#include <vector>

// checking the user path
std::string pathHandler()
{
    std::string path;
    std::cout << "Please enter a path of a file/directory: " << std::endl;
    bool flag = false;
    while(!flag)
    {
        std::cin >> path;
        // check if the current path is exist
        if (std::filesystem::exists(path)) {
            if (std::filesystem::is_directory(path)) {
                std::cout << "This is a directory!" << std::endl;
            }
            else if (std::filesystem::is_regular_file(path)) {
                std::cout << "This is a file!" << std::endl;
            }
            flag = true;
        }
        else {
            std::cout << "Try again..." << std::endl;
            std::cout << "Enter a valid path name: " << std::endl;
        }
    }
    return path;
}

// create and send the first packet -> configuration packet
void createAndSendConfigPacket(boost::asio::ip::udp::socket& socket, const std::string& ipAddress, unsigned short port, std::string& path, std::string& currentName, unsigned long chunkSize) {
    FILE_STORAGE::ConfigPacket configPacket;
    if (std::filesystem::is_directory(path))
        configPacket.set_type(FILE_STORAGE::FileType::DIRECTORY);
    else
    {
        configPacket.set_type(FILE_STORAGE::FileType::FILE);
        std::uint64_t fileSize = std::filesystem::file_size(path);
        configPacket.set_chunks((fileSize % chunkSize) ? long(fileSize/chunkSize) + 1 : fileSize/chunkSize);
        configPacket.set_chunk_size(chunkSize);
    }
    configPacket.set_name(currentName);

    // sending the configuration packet
    socket.send_to(boost::asio::buffer(configPacket.SerializeAsString()),
                   boost::asio::ip::udp::endpoint{boost::asio::ip::make_address(ipAddress), port});
}

// doing serialization to file in the path and send it in multiple chunks
void fileSender(boost::asio::ip::udp::socket& socket, const std::string& ipAddress, unsigned short port, std::string& path, unsigned long chunkSize)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Error while opening the file..." << std::endl;
        return;
    }
    std::string chunkInfo;
    std::vector<char> buffer(chunkSize);
    unsigned long id = 1;
    FILE_STORAGE::Packet packet; // creating packet object
    while(!file.eof())
    {
        file.read(buffer.data(), chunkSize);
        packet.set_id(id);
        packet.set_file_content(std::string(buffer.begin(), buffer.end()));
        std::cout << "Packet ID:  " << packet.id() << std::endl;
        std::cout << "Packet file content in chunk of " << chunkSize << " bytes is: \n" << packet.file_content() << std::endl;
        std::size_t bytesSent = socket.send_to(boost::asio::buffer(packet.SerializeAsString()),
                       boost::asio::ip::udp::endpoint{boost::asio::ip::make_address(ipAddress), port});

        std::cout << "BYTES THAT SENT IN THE " << id << " PACKET ARE: " << bytesSent << " BYTES." << std::endl;
        ++id;
        // reset vector elements
        buffer.assign(buffer.size(), 0);
    }
    file.close();
}
