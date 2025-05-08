#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <filesystem>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include "Protocol.hpp"

#pragma comment(lib, "ws2_32.lib")


class UpdateFilesList
{
public:
    static std::vector<std::string> updateFilesList(const std::string& serverIP, uint16_t port);
};