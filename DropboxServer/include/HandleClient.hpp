#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <filesystem>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include "Protocol.hpp"

#pragma comment(lib, "ws2_32.lib")

#define STORAGE "Storage"


class HandleClient
{
public:
	static bool handleClient(SOCKET clientSock);
};