#include "../include/FileDeletion.hpp"


bool FileDeletion::delFile(const std::string& filename, const std::string& serverIP, uint16_t port)
{
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[-] WSAStartup failed." << std::endl;
        return false;
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "[-] Socket creation failed." << std::endl;
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

    if (connect(sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[-] Connection failed." << std::endl;
        closesocket(sock);
        WSACleanup();
        return false;
    }

    MessageHeader header{
        CommandType::DelFile,
        static_cast<uint32_t>(filename.size()),
        0
    };

    send(sock, reinterpret_cast<const char*>(&header), sizeof(header), 0);
    send(sock, filename.c_str(), filename.size(), 0);

    MessageHeader responseHeader;
    recv(sock, reinterpret_cast<char*>(&responseHeader), sizeof(responseHeader), MSG_WAITALL);

    if (responseHeader.filenameSize == 1) {
        std::cout << "[+] The file has been deleted." << std::endl;
    }
    else {
        std::cerr << "[-] Failed to delete the file on server." << std::endl;
    }

    closesocket(sock);
    WSACleanup();
    return true;
}