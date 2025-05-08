#include "../include/DropboxApp.hpp"


DropboxApp::DropboxApp()
    : serverIP("127.0.0.1"), serverPort(8888),
    mainPath(std::filesystem::current_path().string())
{
    init();
}

DropboxApp::DropboxApp(std::string serverIP, uint16_t serverPort)
    : serverIP(serverIP), serverPort(serverPort),
    mainPath(std::filesystem::current_path().string())
{
    init();
}

void DropboxApp::init()
{
    ctx.antialiasingLevel = 8;
    window.create(sf::VideoMode(WIDTH, HEIGHT), "Dropbox", sf::Style::None, ctx);
    window.setFramerateLimit(60);

    OleInitialize(nullptr);
    HWND hwnd = window.getSystemHandle();
    DragAndDrop* dropTarget = new DragAndDrop(hwnd);
    RegisterDragDrop(hwnd, dropTarget);

    mainT.loadFromFile(mainPath + "\\Graphics\\Textures\\Main.png");
    mainS.setTexture(mainT);
    mainS.setPosition(sf::Vector2f(0.f, 0.f));

    settingsT.loadFromFile(mainPath + "\\Graphics\\Textures\\Settings.png");
    settingsS.setTexture(settingsT);
    settingsS.setPosition(sf::Vector2f(0.f, 130.f));

    pathT.loadFromFile(mainPath + "\\Graphics\\Textures\\Path.png");
    pathS.setTexture(pathT);
    pathS.setPosition(sf::Vector2f(400.f, 130.f));

    dragDropT.loadFromFile(mainPath + "\\Graphics\\Textures\\DragAndDrop.png");
    dragDropS.setTexture(dragDropT);
    dragDropS.setPosition(sf::Vector2f(18.f, 168.f));

    font.loadFromFile(mainPath + "\\Graphics\\Fonts\\Inter\\Inter-Regular.otf");
    successErrorConnect.setFont(font);

    ipPort.setFont(font);
    ipPort.setString(serverIP + ":" + std::to_string(serverPort));
    ipPort.setPosition(sf::Vector2f(143.f, 90.f));
    ipPort.setFillColor(sf::Color(0, 255, 255));
    ipPort.setCharacterSize(16);

    countFiles.setFont(font);
    countFiles.setPosition(sf::Vector2f(105.f, 133.f));
    countFiles.setFillColor(sf::Color(170, 170, 170));
    countFiles.setCharacterSize(16);

    selectedPath.setFont(font);
    if (mainPath.size() >= 40) {
        selectedPath.setString(mainPath.substr(0, 40) + "...");
    }
    else {
        selectedPath.setString(mainPath);
    }
    selectedPath.setPosition(pathS.getPosition() + sf::Vector2f(26.f, 76.f));
    selectedPath.setFillColor(sf::Color(170, 170, 170));
    selectedPath.setCharacterSize(14);

    createButtons();

    fileListView.setViewport(sf::FloatRect(
        18.f / WIDTH,
        168.f / HEIGHT,
        764.f / WIDTH,
        422.f / HEIGHT
    ));
    fileListView.setSize(764.f, 422.f);
    fileListView.setCenter(764.f / 2.f, 422.f / 2.f);

    scrollbarTrack = RoundRectangle(sf::Vector2f(6.f, 422.f), 3);
    scrollbarTrack.setFillColor(sf::Color(47, 47, 47));
    scrollbarTrack.setPosition(789.f, 168.f);

    scrollbarSlider = RoundRectangle(sf::Vector2f(6.f, 422.f), 3);
    scrollbarSlider.setFillColor(sf::Color(65, 65, 65));
    scrollbarSlider.setPosition(789.f, 168.f);

    inputIP.init(mainPath + "\\Graphics\\Textures\\Input.png", font, sf::Vector2f(settingsS.getPosition().x + 16.f, settingsS.getPosition().y + 47.f), "IP Address...");
    inputPort.init(mainPath + "\\Graphics\\Textures\\Input.png", font, sf::Vector2f(settingsS.getPosition().x + 16.f, settingsS.getPosition().y + 122.f), "Port...");

    /*thread_network = std::thread(&DropboxApp::updateFilesList, this);
    thread_network.detach();*/

    downloadPath = mainPath;

    updateFilesList();
}

void DropboxApp::run()
{
    pingThread = std::thread([this]() {
        while (keepPinging) {
            bool nempConnect = isConnected;
            isConnected = this->ping(serverIP, serverPort);
            if (!nempConnect && isConnected) {
                updateFilesList();
            }
            std::this_thread::sleep_for(std::chrono::seconds(4));
        }
    });

    while (window.isOpen()) {
        dragDrop();
        updateTextServer();
        handleEvents();
        render();
    }

    keepPinging = false;
    if (pingThread.joinable()) {
        pingThread.join();
    }
}

void DropboxApp::handleEvents()
{
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        handleEventsButtons(event);
        handleEventsSettings(event);
        handleEventsPath(event);
        handleEventsFileWidgets(event);
        draggingWindow(event);
        draggingSettings(event);
        draggingPath(event);
        scrollFileWidgets(event);
    }
}

void DropboxApp::render()
{
    window.clear(sf::Color::Black);
    
    window.draw(mainS);
    window.draw(ipPort);
    window.draw(countFiles);

    for (auto& button : buttons) {
        if (button.getCategory() == static_cast<int>(CategoryButton::CloseSettings) ||
            button.getCategory() == static_cast<int>(CategoryButton::Apply) ||
            button.getCategory() == static_cast<int>(CategoryButton::ClosePath) ||
            button.getCategory() == static_cast<int>(CategoryButton::SelectPath)) {
            continue;
        }
        button.render(window);
    }

    window.setView(fileListView);
    for (auto& fileWidget : fileWidgets) {
        fileWidget.render(window);
    }
    window.setView(window.getDefaultView());

    if (fileWidgets.size() > 6 && !isDragAndDrop) {
        window.draw(scrollbarTrack);
        window.draw(scrollbarSlider);
    }

    renderSettings();
    renderPath();

    if (isDragAndDrop) {
        window.draw(dragDropS);
    }

    window.display();
}

void DropboxApp::createButtons()
{
    buttons.emplace_back(
        static_cast<int>(CategoryButton::Close),
        sf::Vector2f(750.f, 15.f),
        "Graphics\\Textures\\Buttons\\Close\\Close1.png",
        "Graphics\\Textures\\Buttons\\Close\\Close2.png",
        "Graphics\\Textures\\Buttons\\Close\\Close3.png",
        L"",
        font
    );

    buttons.emplace_back(
        static_cast<int>(CategoryButton::RollUp),
        sf::Vector2f(705.f, 15.f),
        "Graphics\\Textures\\Buttons\\RollUp\\RollUp1.png",
        "Graphics\\Textures\\Buttons\\RollUp\\RollUp2.png",
        "Graphics\\Textures\\Buttons\\RollUp\\RollUp3.png",
        L"",
        font
    );

    buttons.emplace_back(
        static_cast<int>(CategoryButton::Settings),
        sf::Vector2f(18.f, 81.f),
        "Graphics\\Textures\\Buttons\\Settings\\Settings1.png",
        "Graphics\\Textures\\Buttons\\Settings\\Settings2.png",
        "Graphics\\Textures\\Buttons\\Settings\\Settings3.png",
        L"",
        font
    );

    buttons.emplace_back(
        static_cast<int>(CategoryButton::Upload),
        sf::Vector2f(690.f, 84.f),
        "Graphics\\Textures\\Buttons\\Upload\\Upload1.png",
        "Graphics\\Textures\\Buttons\\Upload\\Upload2.png",
        "Graphics\\Textures\\Buttons\\Upload\\Upload3.png",
        L"",
        font
    );

    buttons.emplace_back(
        static_cast<int>(CategoryButton::Update),
        sf::Vector2f(20.f, 127.f),
        "Graphics\\Textures\\Buttons\\Update\\Update1.png",
        "Graphics\\Textures\\Buttons\\Update\\Update2.png",
        "Graphics\\Textures\\Buttons\\Update\\Update3.png",
        L"",
        font
    );

    buttons.emplace_back(
        static_cast<int>(CategoryButton::CloseSettings),
        sf::Vector2f(settingsS.getPosition() + sf::Vector2f(181.f, 4.f)),
        "Graphics\\Textures\\Buttons\\CloseSettings\\Close1.png",
        "Graphics\\Textures\\Buttons\\CloseSettings\\Close2.png",
        "Graphics\\Textures\\Buttons\\CloseSettings\\Close3.png",
        L"",
        font
    );

    buttons.emplace_back(
        static_cast<int>(CategoryButton::Apply),
        sf::Vector2f(settingsS.getPosition() + sf::Vector2f(16.f, 174.f)),
        "Graphics\\Textures\\Buttons\\Apply\\Apply1.png",
        "Graphics\\Textures\\Buttons\\Apply\\Apply2.png",
        "Graphics\\Textures\\Buttons\\Apply\\Apply3.png",
        L"",
        font
    );

    buttons.emplace_back(
        static_cast<int>(CategoryButton::Path),
        sf::Vector2f(sf::Vector2f(602.f, 84.f)),
        "Graphics\\Textures\\Buttons\\Path\\Path1.png",
        "Graphics\\Textures\\Buttons\\Path\\Path2.png",
        "Graphics\\Textures\\Buttons\\Path\\Path3.png",
        L"",
        font
    );

    buttons.emplace_back(
        static_cast<int>(CategoryButton::ClosePath),
        sf::Vector2f(pathS.getPosition() + sf::Vector2f(361.f, 4.f)),
        "Graphics\\Textures\\Buttons\\CloseSettings\\Close1.png",
        "Graphics\\Textures\\Buttons\\CloseSettings\\Close2.png",
        "Graphics\\Textures\\Buttons\\CloseSettings\\Close3.png",
        L"",
        font
    );

    buttons.emplace_back(
        static_cast<int>(CategoryButton::SelectPath),
        sf::Vector2f(pathS.getPosition() + sf::Vector2f(361.f, 74.f)),
        "Graphics\\Textures\\Buttons\\SelectPath\\SelectPath1.png",
        "Graphics\\Textures\\Buttons\\SelectPath\\SelectPath2.png",
        "Graphics\\Textures\\Buttons\\SelectPath\\SelectPath3.png",
        L"",
        font
    );
}

void DropboxApp::draggingWindow(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        bool isButtonCursor = false;
        for (auto& button : buttons) {
            if (button.getCategory() == static_cast<int>(CategoryButton::Close) ||
                button.getCategory() == static_cast<int>(CategoryButton::RollUp)) {
                if (button.cursor()) {
                    isButtonCursor = true;
                }
            }
        }
        if (!isButtonCursor) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (mousePos.y >= 0 && mousePos.y <= 64) {
                isDraggingWindow = true;
                dragWindowOffset = mousePos;
            }
        }
    }

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left) {
        isDraggingWindow = false;
    }

    if (event.type == sf::Event::MouseMoved && isDraggingWindow) {
        sf::Vector2i mouseScreenPos = sf::Mouse::getPosition();
        sf::Vector2i newPos = mouseScreenPos - dragWindowOffset;
        window.setPosition(newPos);
    }
}

void DropboxApp::handleEventsButtons(const sf::Event& event)
{
    for (auto& button : buttons) {
        if (button.getCategory() == static_cast<int>(CategoryButton::CloseSettings) ||
            button.getCategory() == static_cast<int>(CategoryButton::Apply) || 
            button.getCategory() == static_cast<int>(CategoryButton::ClosePath) || 
            button.getCategory() == static_cast<int>(CategoryButton::SelectPath)) {
            continue;
        }
        if ((button.getCategory() == static_cast<int>(CategoryButton::Update) ||
            button.getCategory() == static_cast<int>(CategoryButton::Upload)) &&
            isSettings) {
            continue;
        }

        button.handleEvent(event, window);
        if (button.clicking()) {
            if (button.getCategory() == static_cast<int>(CategoryButton::Close)) {
                window.close();
            }
            else if (button.getCategory() == static_cast<int>(CategoryButton::RollUp)) {
                ShowWindow(window.getSystemHandle(), SW_MINIMIZE);
            }
            else if (button.getCategory() == static_cast<int>(CategoryButton::Settings)) {
                isSettings = !isSettings;
                if (!isSettings) {
                    isSuccessErrorConnecting = false;
                    inputIP.clear();
                    inputPort.clear();
                }
            }
            else if (button.getCategory() == static_cast<int>(CategoryButton::Upload)) {
                std::vector<std::string> filePaths = openFilesDialog();
                for (const auto& filePath : filePaths) {
                    if (filePath != "") {
                        FileUploader::uploadFile(filePath, serverIP, serverPort);
                    }
                }
                if (!filePaths.empty()) {
                    updateFilesList();
                }
            }
            else if (button.getCategory() == static_cast<int>(CategoryButton::Update)) {
                /*thread_network = std::thread(&DropboxApp::updateFilesList, this);
                thread_network.detach();*/
                updateFilesList();
            }
            else if (button.getCategory() == static_cast<int>(CategoryButton::Path)) {
                isPath = !isPath;
            }
        }
    }
}

void DropboxApp::draggingSettings(const sf::Event& event)
{
    const static sf::Vector2f ip_padding(16.f, 47.f);
    const static sf::Vector2f port_padding(16.f, 122.f);

    for (auto& button : buttons) {
        if (button.getCategory() == static_cast<int>(CategoryButton::CloseSettings) ||
            button.getCategory() == static_cast<int>(CategoryButton::Apply)) {
            if (button.cursor()) {
                return;
            }
        }
    }

    if (!isSettings) return;

    switch (event.type) {
    case sf::Event::MouseButtonPressed:
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::IntRect bgRect{
                int(settingsS.getPosition().x),
                int(settingsS.getPosition().y),
                int(settingsS.getGlobalBounds().width),
                int(settingsS.getGlobalBounds().height)
            };
            sf::Vector2i click{ event.mouseButton.x, event.mouseButton.y };
            if (bgRect.contains(click)) {
                isDraggingSettings = true;
                dragSettingsOffset = click - sf::Vector2i(
                    int(settingsS.getPosition().x),
                    int(settingsS.getPosition().y)
                );
            }
        }
        break;

    case sf::Event::MouseButtonReleased:
        if (event.mouseButton.button == sf::Mouse::Left)
            isDraggingSettings = false;
        break;

    case sf::Event::MouseMoved:
        if (isDraggingSettings) {
            sf::Vector2i mousePos{ event.mouseMove.x, event.mouseMove.y };
            sf::Vector2i newPos = mousePos - dragSettingsOffset;

            int winW = window.getSize().x;
            int winH = window.getSize().y;
            int setW = int(settingsS.getGlobalBounds().width);
            int setH = int(settingsS.getGlobalBounds().height);

            newPos.x = std::clamp(newPos.x, 0, winW - setW);
            newPos.y = std::clamp(newPos.y, 64, winH - setH);

            settingsS.setPosition(float(newPos.x), float(newPos.y));
            inputIP.setPosition(
                float(newPos.x + ip_padding.x),
                float(newPos.y + ip_padding.y)
            );
            inputPort.setPosition(
                float(newPos.x + port_padding.x),
                float(newPos.y + port_padding.y)
            );
            for (auto& button : buttons) {
                if (button.getCategory() == static_cast<int>(CategoryButton::CloseSettings)) {
                    button.setPosition(newPos.x + 181.f, newPos.y + 4.f);
                }
                else if (button.getCategory() == static_cast<int>(CategoryButton::Apply)) {
                    button.setPosition(newPos.x + 16.f, newPos.y + 174.f);
                }
            }
            if (isSuccessErrorConnecting) {
                successErrorConnect.setPosition(settingsS.getPosition().x + 109.f, settingsS.getPosition().y + 180.f);
            }
        }
        break;

    default:
        break;
    }
}

void DropboxApp::draggingPath(const sf::Event& event)
{
    for (auto& button : buttons) {
        if (button.getCategory() == static_cast<int>(CategoryButton::ClosePath) ||
            button.getCategory() == static_cast<int>(CategoryButton::SelectPath)) {
            if (button.cursor()) {
                return;
            }
        }
    }

    if (!isPath) return;

    switch (event.type) {
    case sf::Event::MouseButtonPressed:
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::IntRect bgRect{
                int(pathS.getPosition().x),
                int(pathS.getPosition().y),
                int(pathS.getGlobalBounds().width),
                int(pathS.getGlobalBounds().height)
            };
            sf::Vector2i click{ event.mouseButton.x, event.mouseButton.y };
            if (bgRect.contains(click)) {
                isDraggingPath = true;
                dragPathOffset = click - sf::Vector2i(
                    int(pathS.getPosition().x),
                    int(pathS.getPosition().y)
                );
            }
        }
        break;

    case sf::Event::MouseButtonReleased:
        if (event.mouseButton.button == sf::Mouse::Left)
            isDraggingPath = false;
        break;

    case sf::Event::MouseMoved:
        if (isDraggingPath) {
            sf::Vector2i mousePos{ event.mouseMove.x, event.mouseMove.y };
            sf::Vector2i newPos = mousePos - dragPathOffset;

            int winW = window.getSize().x;
            int winH = window.getSize().y;
            int setW = int(pathS.getGlobalBounds().width);
            int setH = int(pathS.getGlobalBounds().height);

            newPos.x = std::clamp(newPos.x, 0, winW - setW);
            newPos.y = std::clamp(newPos.y, 64, winH - setH);

            pathS.setPosition(float(newPos.x), float(newPos.y));
            for (auto& button : buttons) {
                if (button.getCategory() == static_cast<int>(CategoryButton::ClosePath)) {
                    button.setPosition(newPos.x + 361.f, newPos.y + 4.f);
                }
                else if (button.getCategory() == static_cast<int>(CategoryButton::SelectPath)) {
                    button.setPosition(newPos.x + 361.f, newPos.y + 74.f);
                }
            }
            selectedPath.setPosition(newPos.x + 26.f, newPos.y + 76.f);
        }
        break;

    default:
        break;
    }
}

void DropboxApp::handleEventsSettings(const sf::Event& event)
{
    if (isSettings) {
        for (auto& button : buttons) {
            if (button.getCategory() != static_cast<int>(CategoryButton::CloseSettings) && 
                button.getCategory() != static_cast<int>(CategoryButton::Apply)) {
                continue;
            }

            button.handleEvent(event, window);
            if (button.clicking()) {
                if (button.getCategory() == static_cast<int>(CategoryButton::CloseSettings)) {
                    isSettings = false;
                    isSuccessErrorConnecting = false;
                    inputIP.clear();
                    inputPort.clear();
                }

                else if (button.getCategory() == static_cast<int>(CategoryButton::Apply)) {
                    updateSettings();
                }
            }
        }
    }

    inputIP.handleEvent(event);
    inputPort.handleEvent(event);
}

void DropboxApp::renderSettings()
{
    if (isSettings) {
        window.draw(settingsS);
        window.draw(inputIP);
        window.draw(inputPort);
        if (isSuccessErrorConnecting) {
            window.draw(successErrorConnect);
        }

        for (auto& button : buttons) {
            if (button.getCategory() == static_cast<int>(CategoryButton::CloseSettings) ||
                button.getCategory() == static_cast<int>(CategoryButton::Apply)) {
                button.render(window);
            }
        }
    }
}

void DropboxApp::handleEventsPath(const sf::Event& event)
{
    if (isPath) {
        for (auto& button : buttons) {
            if (button.getCategory() != static_cast<int>(CategoryButton::ClosePath) &&
                button.getCategory() != static_cast<int>(CategoryButton::SelectPath)) {
                continue;
            }

            button.handleEvent(event, window);
            if (button.clicking()) {
                if (button.getCategory() == static_cast<int>(CategoryButton::ClosePath)) {
                    isPath = false;
                }

                else if (button.getCategory() == static_cast<int>(CategoryButton::SelectPath)) {
                    choosePath();
                }
            }
        }
    }
}

void DropboxApp::renderPath()
{
    if (isPath) {
        window.draw(pathS);
        window.draw(selectedPath);

        for (auto& button : buttons) {
            if (button.getCategory() == static_cast<int>(CategoryButton::ClosePath) ||
                button.getCategory() == static_cast<int>(CategoryButton::SelectPath)) {
                button.render(window);
            }
        }
    }
}

void DropboxApp::handleEventsFileWidgets(const sf::Event& event)
{
    if (!isSettings && !isPath && !isDragAndDrop) {
        window.setView(fileListView);
        for (auto& fileWidget : fileWidgets) {
            if (fileWidget.handleEvents(event, window, downloadPath)) {
                updateFilesList();
            }
        }
        window.setView(window.getDefaultView());
    }
}

bool DropboxApp::ping(const std::string& tempIP, uint16_t tempPort)
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
    serverAddr.sin_port = htons(tempPort);
    inet_pton(AF_INET, tempIP.c_str(), &serverAddr.sin_addr);

    if (connect(sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[-] Connection failed." << std::endl;
        closesocket(sock);
        WSACleanup();
        return false;
    }

    MessageHeader header{
        CommandType::Ping
    };

    send(sock, reinterpret_cast<const char*>(&header), sizeof(header), 0);

    char response[16] = {};
    int bytes = recv(sock, response, sizeof(response) - 1, 0);
    if (bytes > 0) {
        std::cout << "[+] Server responded: " << response << std::endl;
    }
    else {
        std::cerr << "[-] No response from server." << std::endl;
    }

    closesocket(sock);
    WSACleanup();
    return true;
}

void DropboxApp::updateSettings()
{
    std::string newIP = inputIP.getInput();
    std::string newPort = inputPort.getInput();

    bool isError = false;
    if (!newIP.empty() && !newPort.empty()) {
        if (ping(newIP, std::stoi(newPort))) {
            serverIP = newIP;
            serverPort = std::stoi(newPort);
            ipPort.setString(serverIP + ":" + std::to_string(serverPort));

            isSuccessErrorConnecting = true;
            successErrorConnect.setString("Successfully");
            successErrorConnect.setCharacterSize(16);
            successErrorConnect.setFillColor(sf::Color(0, 172, 193));
            successErrorConnect.setPosition(settingsS.getPosition().x + 109.f, settingsS.getPosition().y + 180.f);

            isConnected = true;

            /*thread_network = std::thread(&DropboxApp::updateFilesList, this);
            thread_network.detach();*/
            updateFilesList();
        }
        else {
            isError = true;
        }
    }
    else {
        isError = true;
    }

    if (isError) {
        isSuccessErrorConnecting = true;
        successErrorConnect.setString("Error connect");
        successErrorConnect.setCharacterSize(16);
        successErrorConnect.setFillColor(sf::Color(255, 86, 86));
        successErrorConnect.setPosition(settingsS.getPosition().x + 109.f, settingsS.getPosition().y + 180.f);
    }
}

void DropboxApp::updateFilesList()
{
    filesList = UpdateFilesList::updateFilesList(serverIP, serverPort);
    if (filesList.empty()) {
        isConnected = false;
    }
    if (filesList.size() == 1) {
        if (filesList[0] == "[Empty]") {
            filesList.clear();
            fileWidgets.clear();
            countFiles.setString("(0)");
            return;
        }
    }

    filesList.erase(std::remove_if(filesList.begin(), filesList.end(),
        [](const std::string& s) { return s.empty(); }), filesList.end());

    fileWidgets.clear();

    countFiles.setString("(" + std::to_string(filesList.size()) + ")");

    float startY = 168.f;
    const float paddingX = 18.f;
    const float verticalSpacing = 10.f;

    for (std::size_t i = 0; i < filesList.size(); ++i) {
        float posY = startY + i * (62.f + verticalSpacing);
        sf::Vector2f position(paddingX, posY);

        std::cout << filesList[i] << std::endl;
        fileWidgets.emplace_back(filesList[i], position, mainPath, i + 1, serverIP, serverPort);
    }

    float contentHeight = filesList.size() * (62.f + 10.f);
    float maxScroll = std::max(0.f, contentHeight - 422.f);
    scrollFileWidgetsOffset = std::clamp(scrollFileWidgetsOffset, 0.f, maxScroll);

    fileListView.setCenter(
        18.f + 764.f / 2.f,
        168.f + 422.f / 2.f + scrollFileWidgetsOffset
    );

    updateScrollFileWidgets();
}

void DropboxApp::scrollFileWidgets(const sf::Event& event)
{
    if (event.type == sf::Event::MouseWheelScrolled &&
        event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        if (fileWidgets.size() > 6) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::IntRect scrollArea(18, 168, 775, 422);
            if (!scrollArea.contains(mousePos)) {
                return;
            }

            float delta = event.mouseWheelScroll.delta * -30.f;

            float contentHeight = filesList.size() * (62.f + 10.f);
            float maxScroll = std::max(0.f, contentHeight - 422.f);

            scrollFileWidgetsOffset += delta;
            scrollFileWidgetsOffset = std::clamp(scrollFileWidgetsOffset, 0.f, maxScroll);

            fileListView.setCenter(
                18.f + 764.f / 2.f,
                168.f + 422.f / 2.f + scrollFileWidgetsOffset
            );

            updateScrollFileWidgets();
        }
    }

    else if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        if (fileWidgets.size() > 6) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (scrollbarSlider.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                isDraggingSlider = true;
                dragSliderOffsetY = mousePos.y - scrollbarSlider.getPosition().y;
            }
        }
    }

    else if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left) {
        isDraggingSlider = false;
    }

    else if (event.type == sf::Event::MouseMoved && isDraggingSlider) {
        float visibleHeight = 422.f;
        float contentHeight = filesList.size() * (62.f + 10.f);

        if (contentHeight <= visibleHeight)
            return;

        float maxScroll = contentHeight - visibleHeight;
        float sliderTrackTop = 168.f;
        float sliderTrackHeight = visibleHeight;
        float sliderHeight = scrollbarSlider.getSize().y;

        float mouseY = static_cast<float>(sf::Mouse::getPosition(window).y);
        float newSliderY = mouseY - dragSliderOffsetY;

        newSliderY = std::clamp(newSliderY, sliderTrackTop, sliderTrackTop + sliderTrackHeight - sliderHeight);

        float scrollRatio = (newSliderY - sliderTrackTop) / (sliderTrackHeight - sliderHeight);
        scrollFileWidgetsOffset = scrollRatio * maxScroll;

        fileListView.setCenter(
            18.f + 764.f / 2.f,
            168.f + 422.f / 2.f + scrollFileWidgetsOffset
        );

        updateScrollFileWidgets();
    }
}

void DropboxApp::updateScrollFileWidgets()
{
    float contentHeight = filesList.size() * (62.f + 10.f);
    float visibleHeight = 422.f;

    if (contentHeight <= visibleHeight) {
        scrollbarSlider.setSize({ 6.f, visibleHeight });
        scrollbarSlider.setPosition(789.f, 168.f);
        return;
    }

    float viewRatio = visibleHeight / contentHeight;
    float sliderHeight = viewRatio * visibleHeight;

    float maxScroll = contentHeight - visibleHeight;
    float scrollRatio = scrollFileWidgetsOffset / maxScroll;
    float sliderY = 168.f + scrollRatio * (visibleHeight - sliderHeight);

    scrollbarSlider.setSize({ 6.f, sliderHeight });
    scrollbarSlider.setPosition(789.f, sliderY);
}

void DropboxApp::choosePath()
{
    std::string newDownloadPath = openSelectPath();
    if (newDownloadPath != "") {
        downloadPath = newDownloadPath;
        selectedPath.setString(downloadPath);
    }
}

void DropboxApp::updateTextServer()
{
    if (isConnected) {
        ipPort.setFillColor(sf::Color(0, 255, 255));
    }
    else {
        ipPort.setFillColor(sf::Color(255, 86, 86));
    }
}

void DropboxApp::dragDrop()
{
    for (const auto& pathDragAndDrop : pathsDragAndDrop) {
        if (pathDragAndDrop != "") {
            FileUploader::uploadFile(pathDragAndDrop, serverIP, serverPort);
        }
    }

    if (!pathsDragAndDrop.empty()) {
        updateFilesList();
    }

    if (!pathsDragAndDrop.empty()) {
        pathsDragAndDrop.clear();
    }
}

std::vector<std::string> DropboxApp::openFilesDialog()
{
    std::vector<std::string> files;
    char buffer[8192] = "";

    OPENFILENAMEA ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "All Files\0*.*\0Text Files\0*.txt\0";
    ofn.lpstrFile = buffer;
    ofn.nMaxFile = sizeof(buffer);
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    if (GetOpenFileNameA(&ofn)) {
        char* ptr = buffer;
        std::string directory = ptr;
        ptr += directory.size() + 1;

        if (*ptr == '\0') {
            files.push_back(directory);
        }
        else {
            while (*ptr) {
                files.push_back(directory + "\\" + ptr);
                ptr += strlen(ptr) + 1;
            }
        }
    }

    return files;
}

std::string DropboxApp::openSelectPath()
{
    BROWSEINFOA bi = { 0 };
    bi.lpszTitle = "Select folder";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
    if (pidl != nullptr) {
        char path[MAX_PATH];
        if (SHGetPathFromIDListA(pidl, path)) {
            CoTaskMemFree(pidl);
            return std::string(path);
        }
        CoTaskMemFree(pidl);
    }
    return "";
}