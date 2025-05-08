#include "../include/UpdateFilesList.hpp"


std::vector<std::string> UpdateFilesList::updateFilesList(const std::string& serverIP, uint16_t port)
{
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[-] WSAStartup failed." << std::endl;
        return {};
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "[-] Socket creation failed." << std::endl;
        WSACleanup();
        return {};
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

    if (connect(sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[-] Connection failed." << std::endl;
        closesocket(sock);
        WSACleanup();
        return {};
    }

    MessageHeader header{
        CommandType::ListFiles,
        0, 
        0
    };

    send(sock, reinterpret_cast<const char*>(&header), sizeof(header), 0);

    uint32_t fileCount = 0;
    recv(sock, reinterpret_cast<char*>(&fileCount), sizeof(fileCount), MSG_WAITALL);

    std::vector<std::string> filesList;
    if (fileCount == 0) {
        filesList.push_back("[Empty]");
    }
    else {
        for (uint32_t i = 0; i < fileCount; ++i) {
            uint32_t nameLen = 0;
            recv(sock, reinterpret_cast<char*>(&nameLen), sizeof(nameLen), MSG_WAITALL);

            std::string name(nameLen, '\0');
            recv(sock, name.data(), nameLen, MSG_WAITALL);

            filesList.push_back(name);
        }
    }

    closesocket(sock);
    WSACleanup();
    return filesList;
}