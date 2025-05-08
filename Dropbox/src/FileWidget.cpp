#include "../include/FileWidget.hpp"


sf::Texture FileWidget::fileT;

FileWidget::FileWidget(const std::string& nameFile, const sf::Font& font, const sf::Vector2f& position, const std::string& mainPath, 
	size_t count, const std::string& serverIP, uint16_t serverPort)
	: nameFile(nameFile), position(position), serverIP(serverIP), serverPort(serverPort)
{
	if (fileT.getSize().x == 0) {
		if (!fileT.loadFromFile(mainPath + "\\Graphics\\Textures\\File.png")) {
			std::cerr << "[-] Failed to load texture File.png" << std::endl;
		}
	}
	fileS.setTexture(fileT);
	fileS.setPosition(position);

	std::wstring nameFileTemp;
	{
		std::wstring nameFull = cp1251_to_wstring(std::to_string(count) + ". " + nameFile);
		nameFull = remove_nonprintable(nameFull);

		size_t dotPos = nameFull.find_last_of(L'.');
		std::wstring extension = (dotPos != std::wstring::npos) ? nameFull.substr(dotPos) : L"";

		nameFileTemp = nameFull;
		if (nameFull.size() > 64) {
			size_t maxBaseLength = 64;
			if (extension.size() + 3 < 64) {
				maxBaseLength = 64 - extension.size() - 3;
			}
			else {
				maxBaseLength = 0;
			}

			nameFileTemp = nameFull.substr(0, maxBaseLength) + L".." + extension;
		}
	}

	textFile.setString(nameFileTemp);
	textFile.setFont(font);
	textFile.setCharacterSize(16);
	textFile.setFillColor(sf::Color::White);
	textFile.setPosition(position.x + 58.f, position.y + 22.f);

	loadFile = Button(
		static_cast<int>(CategoryButtonFile::LoadFile),
		sf::Vector2f(position.x + 676.f, position.y + 16.f),
		mainPath + "\\Graphics\\Textures\\Buttons\\LoadFile\\LoadFile1.png",
		mainPath + "\\Graphics\\Textures\\Buttons\\LoadFile\\LoadFile2.png",
		mainPath + "\\Graphics\\Textures\\Buttons\\LoadFile\\LoadFile3.png",
		L"",
		font
	);

	delFile = Button(
		static_cast<int>(CategoryButtonFile::DelFile),
		sf::Vector2f(position.x + 718.f, position.y + 16.f),
		mainPath + "\\Graphics\\Textures\\Buttons\\DelFile\\DelFile1.png",
		mainPath + "\\Graphics\\Textures\\Buttons\\DelFile\\DelFile2.png",
		mainPath + "\\Graphics\\Textures\\Buttons\\DelFile\\DelFile3.png",
		L"",
		font
	);
}

void FileWidget::setPosition(const sf::Vector2f& newPosition)
{
	position = newPosition;
	fileS.setPosition(position);
	textFile.setPosition(position.x + 58.f, position.y + 22.f);
}

sf::Vector2f FileWidget::getPosition() const
{
	return position;
}

bool FileWidget::handleEvents(const sf::Event& event, sf::RenderWindow& window, const std::string& downloadPath)
{
	loadFile.handleEvent(event, window);
	if (loadFile.clicking()) {
		return FileDownloader::downloadFile(nameFile, serverIP, serverPort, downloadPath);
	}

	delFile.handleEvent(event, window);
	if (delFile.clicking()) {
		return FileDeletion::delFile(nameFile, serverIP, serverPort);
	}

	return false;
}

void FileWidget::render(sf::RenderWindow& window)
{
	window.draw(fileS);
	window.draw(textFile);
	loadFile.render(window);
	delFile.render(window);
}

std::wstring FileWidget::cp1251_to_wstring(const std::string& cp1251_str)
{
	int wide_len = MultiByteToWideChar(1251, 0, cp1251_str.c_str(), -1, nullptr, 0);
	if (wide_len <= 0) return L"";
	std::wstring wide_str(wide_len, 0);
	MultiByteToWideChar(1251, 0, cp1251_str.c_str(), -1, &wide_str[0], wide_len);
	return wide_str;
}

std::wstring FileWidget::remove_nonprintable(const std::wstring& input)
{
	std::wstring output;
	for (wchar_t ch : input) {
		if (ch >= 32 && ch != 127 && ch != 0xFEFF) {
			output += ch;
		}
	}
	return output;
}