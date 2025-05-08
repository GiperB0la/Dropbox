#include "../include/HandleClient.hpp"


bool HandleClient::handleClient(SOCKET clientSock)
{
    MessageHeader header;
    int result = recv(clientSock, reinterpret_cast<char*>(&header), sizeof(header), MSG_WAITALL);
    if (result != sizeof(header)) {
        std::cerr << "[-] Couldn't read the title." << std::endl;
        return false;
    }

    if (header.command == CommandType::UploadFile) {
        std::string filename(header.filenameSize, '\0');
        result = recv(clientSock, filename.data(), static_cast<int>(filename.size()), MSG_WAITALL);
        if (result != static_cast<int>(filename.size())) {
            std::cerr << "[-] Couldn't read the file name." << std::endl;
            return false;
        }

        std::filesystem::path storagePath = STORAGE;
        std::filesystem::create_directory(storagePath);
        std::filesystem::path fullPath = storagePath / filename;

        std::ofstream out(fullPath, std::ios::binary);
        if (!out) {
            std::cerr << "[-] Error opening a file on the server." << std::endl;
            return false;
        }

        uint64_t received = 0;
        char buffer[4096];

        while (received < header.fileSize) {
            int bytes = recv(clientSock, buffer, sizeof(buffer), 0);
            if (bytes <= 0) break;
            out.write(buffer, bytes);
            received += bytes;
        }

        std::cout << "[+] File received: " << filename << std::endl;
    }

    else if (header.command == CommandType::DownloadFile) {
        std::string filename(header.filenameSize, '\0');
        result = recv(clientSock, filename.data(), static_cast<int>(filename.size()), MSG_WAITALL);
        if (result != static_cast<int>(filename.size())) {
            std::cerr << "[-] Couldn't read the file name." << std::endl;
            return false;
        }

        std::filesystem::path storagePath = STORAGE;
        std::filesystem::create_directory(storagePath);
        std::filesystem::path fullPath = storagePath / filename;

        std::ifstream in(fullPath, std::ios::binary | std::ios::ate);
        if (!in) {
            std::cerr << "[-] File not found: " << filename << std::endl;
            MessageHeader errorResp{ CommandType::DownloadFile, 0, 0 };
            send(clientSock, reinterpret_cast<const char*>(&errorResp), sizeof(errorResp), 0);
            return false;
        }

        uint64_t fileSize = in.tellg();
        in.seekg(0);

        MessageHeader resp{ CommandType::DownloadFile, 0, fileSize };
        send(clientSock, reinterpret_cast<const char*>(&resp), sizeof(resp), 0);

        char buffer[4096];
        while (!in.eof()) {
            in.read(buffer, sizeof(buffer));
            send(clientSock, buffer, static_cast<int>(in.gcount()), 0);
        }

        std::cout << "[+] The file has been sent: " << filename << std::endl;
    }

    else if (header.command == CommandType::ListFiles) {
        std::filesystem::path storagePath = STORAGE;
        std::vector<std::string> files;

        for (auto& entry : std::filesystem::directory_iterator(storagePath)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().filename().string());
            }
        }

        uint32_t fileCount = static_cast<uint32_t>(files.size());
        send(clientSock, reinterpret_cast<const char*>(&fileCount), sizeof(fileCount), 0);

        for (const auto& name : files) {
            uint32_t len = static_cast<uint32_t>(name.size());
            send(clientSock, reinterpret_cast<const char*>(&len), sizeof(len), 0);
            send(clientSock, name.c_str(), len, 0);
        }

        std::cout << "[+] A list of files has been sent (" << fileCount << " count)" << std::endl;
    }

    else if (header.command == CommandType::Ping) {
        const char* response = "OK";
        send(clientSock, response, static_cast<int>(strlen(response)), 0);
        std::cout << "[+] Test response sent." << std::endl;
    }

    else if (header.command == CommandType::DelFile) {
        std::string filename(header.filenameSize, '\0');
        result = recv(clientSock, filename.data(), static_cast<int>(filename.size()), MSG_WAITALL);
        if (result != static_cast<int>(filename.size())) {
            std::cerr << "[-] Couldn't read the file name." << std::endl;
            return false;
        }

        std::filesystem::path storagePath = STORAGE;
        std::filesystem::create_directory(storagePath);
        std::filesystem::path fullPath = storagePath / filename;

        if (!std::filesystem::exists(fullPath)) {
            std::cerr << "[-] File not found: " << filename << std::endl;

            MessageHeader errorResp{
                CommandType::DelFile,
                0,
                0
            };
            send(clientSock, reinterpret_cast<const char*>(&errorResp), sizeof(errorResp), 0);
            return false;
        }

        try {
            std::filesystem::remove(fullPath);

            std::cout << "[+] Deleted: " << filename << "\n";
            MessageHeader okResp{
                CommandType::DelFile,
                1,
                0
            };
            send(clientSock, reinterpret_cast<const char*>(&okResp), sizeof(okResp), 0);
        }
        catch (const std::exception& e) {
            std::cerr << "[-] Exception during file deletion: " << e.what() << "\n";
            MessageHeader errorResp{
                CommandType::DelFile,
                0,
                0
            };
            send(clientSock, reinterpret_cast<const char*>(&errorResp), sizeof(errorResp), 0);
            return false;
        }
    }

    return true;
}