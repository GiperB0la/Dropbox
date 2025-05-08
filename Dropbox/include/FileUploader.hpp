#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <filesystem>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include "Protocol.hpp"

#pragma comment(lib, "ws2_32.lib")


class FileUploader
{
public:
    static bool uploadFile(const std::string& filepath, const std::string& serverIP, uint16_t port);
};