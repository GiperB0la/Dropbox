#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <SFML/Graphics.hpp>
#include "Widgets.hpp"
#include "FileDownloader.hpp"
#include "FileUploader.hpp"
#include "UpdateFilesList.hpp"
#include "FileWidget.hpp"
#include "DragAndDrop.hpp"
#include <windows.h>
#include <shlobj.h>

#ifdef _DEBUG
    #pragma comment(lib, "sfml-graphics-d.lib")
    #pragma comment(lib, "sfml-window-d.lib")
    #pragma comment(lib, "sfml-system-d.lib")
#else
    #pragma comment(lib, "sfml-graphics.lib")
    #pragma comment(lib, "sfml-window.lib")
    #pragma comment(lib, "sfml-system.lib")
#endif

#define WIDTH 800
#define HEIGHT 600


enum class CategoryButton
{
    Close,
    RollUp,
    Settings,
    Upload,
    Update,
    Apply,
    CloseSettings,
    Path,
    ClosePath,
    SelectPath
};

class DropboxApp
{
public:
    DropboxApp();
    DropboxApp(std::string serverIP, uint16_t serverPort);

public:
    void run();

private:
    void init();
    void handleEvents();
    void render();

private:
    void createButtons();
    void draggingWindow(const sf::Event& event);
    void handleEventsButtons(const sf::Event& event); 
    void updateFilesList();
    void draggingSettings(const sf::Event& event);
    void draggingPath(const sf::Event& event);
    void handleEventsSettings(const sf::Event& event);
    void handleEventsPath(const sf::Event& event);
    void handleEventsFileWidgets(const sf::Event& event);
    void renderSettings();
    void renderPath();
    void updateSettings();
    void scrollFileWidgets(const sf::Event& event);
    void updateScrollFileWidgets();
    void choosePath();
    void updateTextServer();
    void dragDrop();
    bool ping(const std::string& tempIP, uint16_t tempPort);
    std::vector<std::string> openFilesDialog();
    std::string openSelectPath();

private:
    sf::ContextSettings ctx;
    sf::RenderWindow window;
    sf::Font font;
    bool isDraggingWindow = false;
    bool isDraggingSettings = false;
    bool isDraggingPath = false;
    sf::Vector2i dragWindowOffset;
    sf::Vector2i dragSettingsOffset;
    sf::Vector2i dragPathOffset;
    sf::View fileListView;
    float scrollFileWidgetsOffset = 0.f;
    bool isDraggingSlider = false;
    float dragSliderOffsetY = 0.f;
    const std::string mainPath;
    std::string downloadPath;

private:
    sf::Texture mainT, settingsT, pathT, dragDropT;
    sf::Sprite mainS, settingsS, pathS, dragDropS;
    sf::Text ipPort, countFiles, successErrorConnect, selectedPath;
    std::vector<Button> buttons;
    IPInputField inputIP;
    PortInputField inputPort;
    RoundRectangle scrollbarTrack;
    RoundRectangle scrollbarSlider;

private:
    std::string serverIP;
    uint16_t serverPort;

private:
    std::thread thread_network;
    std::mutex loadedFilesMutex;
    bool isSettings = false;
    bool isPath = false;
    bool isSuccessErrorConnecting = false;
    std::vector<std::string> filesList;
    std::vector<FileWidget> fileWidgets;
    std::atomic<bool> keepPinging = true, isConnected = false;
    std::thread pingThread;
};