#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <filesystem>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include "Protocol.hpp"

#pragma comment(lib, "ws2_32.lib")


class FileDownloader
{
public:
    static bool downloadFile(const std::string& filename, const std::string& serverIP, uint16_t port, const std::string& path);
};