#include "../include/DropboxServer.hpp"


void DropboxServer::start(uint16_t port)
{
    WSADATA wsaData;
    SOCKET serverSock = INVALID_SOCKET;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        std::cerr << "[-] WSAStartup failed." << std::endl;
        return;
    }

    serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSock == INVALID_SOCKET) {
        std::cerr << "[-] Socket creation failed." << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[-] Bind failed." << std::endl;
        closesocket(serverSock);
        WSACleanup();
        return;
    }

    if (listen(serverSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "[-] Listen failed." << std::endl;
        closesocket(serverSock);
        WSACleanup();
        return;
    }

    std::cout << "[+] The server is listening on the port " << port << std::endl;

    if (!std::filesystem::exists(STORAGE)) {
        if (!std::filesystem::create_directory(STORAGE)) {
            std::cerr << "[-] Error when creating a folder." << std::endl;
            return;
        }
    }

    while (true) {
        SOCKET clientSock = accept(serverSock, nullptr, nullptr);
        if (clientSock == INVALID_SOCKET) {
            std::cerr << "[-] Accept failed." << std::endl;
            continue;
        }

        std::cout << "[+] The client is connected." << std::endl;
        HandleClient::handleClient(clientSock);
        closesocket(clientSock);
    }

    closesocket(serverSock);
    WSACleanup();
}