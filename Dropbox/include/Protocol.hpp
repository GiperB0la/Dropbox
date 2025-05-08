#pragma once
#include <iostream>


enum class CommandType
{
    UploadFile,
    DownloadFile,
    ListFiles,
    Ping,
    DelFile
};

struct MessageHeader
{
    CommandType command;
    uint32_t filenameSize = 0;
    uint64_t fileSize = 0;
};