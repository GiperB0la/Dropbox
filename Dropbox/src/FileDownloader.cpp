#include "../include/FileDownloader.hpp"


bool FileDownloader::downloadFile(const std::string& filename, const std::string& serverIP, uint16_t port, const std::string& path)
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
        CommandType::DownloadFile,
        static_cast<uint32_t>(filename.size()),
        0
    };

    send(sock, reinterpret_cast<const char*>(&header), sizeof(header), 0);
    send(sock, filename.c_str(), filename.size(), 0);

    MessageHeader responseHeader;
    recv(sock, reinterpret_cast<char*>(&responseHeader), sizeof(responseHeader), MSG_WAITALL);

    if (responseHeader.fileSize == 0) {
        std::cerr << "[-] File not found or error." << std::endl;
        closesocket(sock);
        WSACleanup();
        return false;
    }

    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
        std::cerr << "[-] Path does not exist or is not a directory: " << path << std::endl;
        closesocket(sock);
        WSACleanup();
        return false;
    }
    std::filesystem::path outPath = std::filesystem::path(path) / filename;
    std::ofstream fileOut(outPath, std::ios::binary);
    uint64_t received = 0;
    char buffer[4096];

    while (received < responseHeader.fileSize) {
        int bytes = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes <= 0) break;
        fileOut.write(buffer, bytes);
        received += bytes;
    }

    std::cout << "[+] The file has been downloaded." << std::endl;

    closesocket(sock);
    WSACleanup();
    return true;
}