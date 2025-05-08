#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <filesystem>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include "Protocol.hpp"
#include "HandleClient.hpp"

#pragma comment(lib, "ws2_32.lib")


class DropboxServer
{
public:
    static void start(uint16_t port = 8888);
};