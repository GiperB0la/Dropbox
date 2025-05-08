#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Widgets.hpp"
#include <string>
#include "FileDownloader.hpp"
#include "FileDeletion.hpp"


enum class CategoryButtonFile
{
    LoadFile,
    DelFile
};

class FileWidget
{
public:
    FileWidget(const std::string& nameFile, const sf::Font& font, const sf::Vector2f& position, const std::string& mainPath, 
        size_t count, const std::string& serverIP, uint16_t serverPort);

public:
    void setPosition(const sf::Vector2f& newPosition);
    sf::Vector2f getPosition() const;
    bool handleEvents(const sf::Event& event, sf::RenderWindow& window, const std::string& downloadPath);
    void render(sf::RenderWindow& window);

private:
    static std::wstring cp1251_to_wstring(const std::string& cp1251_str);
    static std::wstring remove_nonprintable(const std::wstring& input);

private:
    static sf::Texture fileT;
    sf::Sprite fileS;
    sf::Text textFile;
    sf::Vector2f position;
    std::string nameFile;
    Button loadFile;
    Button delFile;
    std::string serverIP;
    uint16_t serverPort;
};