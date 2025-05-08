#include "../include/FileUploader.hpp"


bool FileUploader::uploadFile(const std::string& filepath, const std::string& serverIP, uint16_t port)
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

    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "[-] Cannot open file." << std::endl;
        closesocket(sock);
        WSACleanup();
        return false;
    }

    std::string filename = std::filesystem::path(filepath).filename().string();
    std::streamsize fileSize = file.tellg();
    file.seekg(0);

    MessageHeader header{
        CommandType::UploadFile,
        static_cast<uint32_t>(filename.size()),
        static_cast<uint64_t>(fileSize)
    };

    send(sock, reinterpret_cast<const char*>(&header), sizeof(header), 0);
    send(sock, filename.c_str(), filename.size(), 0);

    char buffer[4096];
    while (file) {
        file.read(buffer, sizeof(buffer));
        send(sock, buffer, static_cast<int>(file.gcount()), 0);
    }

    std::cout << "[+] The file has been sent." << std::endl;

    closesocket(sock);
    WSACleanup();
    return true;
}