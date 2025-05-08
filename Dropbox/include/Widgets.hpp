#pragma once
#include <iostream>
#include <regex>
#include <SFML/Graphics.hpp>


// Widget

class Widget
{
public:
	virtual ~Widget() = default;
	virtual void handleEvent(const sf::Event& event, sf::RenderWindow& window) = 0;
	virtual void render(sf::RenderWindow& window) = 0;
	virtual sf::Vector2f getPosition() = 0;
	virtual void setPosition(const sf::Vector2f& position) = 0;
};


// Button

class Button : public Widget
{
public:
	Button() = default;
	Button(
		const size_t category,
		const sf::Vector2f& position,
		const std::string& normal_condition,
		const std::string& hovered_condition,
		const std::string& pressed_condition,
		const std::wstring& text,
		const sf::Font& font,
		size_t size_text = 14,
		const sf::Color color_text_normal = sf::Color::White,
		const sf::Color color_text_hovered = sf::Color::White,
		const sf::Color color_text_pressed = sf::Color::White,
		const sf::Vector2f& position_text = sf::Vector2f()
	);
	~Button() = default;

public:
	void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
	void render(sf::RenderWindow& window) override;
	virtual bool clicking();
	virtual int getCategory();
	sf::Vector2f getPosition() override;
	void setPosition(const sf::Vector2f& position) override;
	void setPosition(float positionX, float positionY);
	bool cursor();
	void setTexture(int state, const std::string& newTexture);
	void updateCursor(sf::RenderWindow& window);
	void setScale(float scaleX, float scaleY);

protected:
	sf::Sprite button;
	size_t category;
	sf::Vector2f position;
	sf::Texture normal_condition_texture;
	sf::Texture hovered_condition_texture;
	sf::Texture pressed_condition_texture;

protected:
	sf::Text text;
	sf::Font font;
	size_t size_text;
	sf::Vector2f position_text;
	sf::Color color_text_normal;
	sf::Color color_text_hovered;
	sf::Color color_text_pressed;

protected:
	enum class State { Normal, Hovered, Pressed };
	State state;

protected:
	bool click = false;
	bool isCursor = false;
};


// CheckButton

class CheckButton : public Button
{
public:
	CheckButton() = default;
	CheckButton(
		const size_t category,
		const sf::Vector2f& position,
		const std::string& normal_condition,
		const std::string& hovered_condition,
		const std::string& pressed_condition,
		const std::wstring& text,
		const sf::Font& font,
		size_t size_text = 14,
		const sf::Color color_text_normal = sf::Color::White,
		const sf::Color color_text_hovered = sf::Color::White,
		const sf::Color color_text_pressed = sf::Color::White,
		const sf::Vector2f& position_text = sf::Vector2f()
	);
	~CheckButton() = default;

public:
	void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
	bool clicking() override;
};


// RadioButton

class RadioButton : public Button
{
public:
	RadioButton(
		std::vector<RadioButton*>& group,
		const size_t category,
		const sf::Vector2f& position,
		const std::string& normal_condition,
		const std::string& hovered_condition,
		const std::string& pressed_condition,
		const std::wstring& text,
		const sf::Font& font,
		size_t size_text = 14,
		const sf::Color color_text_normal = sf::Color::White,
		const sf::Color color_text_hovered = sf::Color::White,
		const sf::Color color_text_pressed = sf::Color::White,
		const sf::Vector2f& position_text = sf::Vector2f());

public:
	void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
	void render(sf::RenderWindow& window) override;
	void resetSelection();
	std::wstring getText();
	bool isSelected() const { return selected; }

private:
	void setSelected(bool value);

private:
	std::wstring text;
	bool selected = false;
	std::vector<RadioButton*>& group;
};


// InputField

class InputField : public Widget
{
public:
	InputField(
		const sf::Vector2f& position,
		const std::string& texture_path,
		const sf::Font& font,
		const std::wstring& text,
		size_t limitation,
		sf::RenderWindow& window
	);

public:
	void handleEvent(const sf::Event& event, sf::RenderWindow& window);
	void render(sf::RenderWindow& window) override;
	sf::Vector2f getPosition() override;
	void setPosition(const sf::Vector2f& position) override;
	std::wstring getText() const;
	void setText(const std::string& text);
	bool isSubmitted() const;
	void resetSubmission();

private:
	void updateDisplayedText();
	void updateCursorPosition();

private:
	sf::Sprite sprite;
	sf::Texture texture;
	sf::Vector2f position;
	sf::Text text;
	std::wstring fullText;
	std::wstring defaultText;
	bool isActive;
	bool submitted;
	size_t limitation;
	size_t cursorPosition;
	sf::RectangleShape cursor;
	sf::Clock cursorTimer;
	bool cursorVisible;
};


// IPInputField

class IPInputField : public sf::Drawable
{
public:
	IPInputField(const std::string& textureFile, const sf::Font& font, sf::Vector2f pos, const std::string placeholder = "", float padding = 10.f, unsigned int charSize = 14);

public:
	void setPosition(float x, float y);
	void handleEvent(const sf::Event& event);
	const std::string& getInput() const;
	bool isValid() const;
	void clear();

private:
	bool validatePartial(const std::string& s) const;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void updateDisplay();

private:
	sf::Texture texture_;
	sf::Sprite sprite_;
	sf::Text text_;
	std::string input_;
	std::string placeholder_;
	bool hasFocus_;
	const float padding_ = 10.f;
	sf::Vector2f position;
};


// PortInputField

class PortInputField : public sf::Drawable
{
public:
	PortInputField(const std::string& textureFile, const sf::Font& font, sf::Vector2f pos, const std::string placeholder = "", float padding = 10.f, unsigned int charSize = 14);

public:
	void setPosition(float x, float y);
	void handleEvent(const sf::Event& event);
	const std::string& getInput() const;
	bool isValid() const;
	void clear();

private:
	bool validatePartial(const std::string& s) const;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void updateDisplay();

private:
	sf::Texture texture_;
	sf::Sprite sprite_;
	sf::Text text_;
	std::string input_;
	std::string placeholder_;
	bool hasFocus_;
	const float padding_ = 10.f;
	sf::Vector2f position;
};


// Squircle

class Squircle : public sf::Shape
{
public:
	Squircle(float radius = 0.f, float n = 4.f, std::size_t resolution = 20);

public:
	void setRadius(float radius);
	float getRadius() const;
	void setN(float p);
	float getN() const;
	void setResolution(std::size_t resolution);
	std::size_t getResolution() const;
	std::size_t getPointCount() const override;
	sf::Vector2f getPoint(std::size_t index) const override;

private:
	float m_radius;
	float m_n;
	std::size_t m_resolution;
};


// RoundRectangle

class RoundRectangle : public sf::Shape
{
public:
	RoundRectangle(sf::Vector2f size = { 0, 0 }, float radius = 0.f);

public:
	void setSize(sf::Vector2f size);
	sf::Vector2f getSize() const;
	void setRadius(float radius);
	float getRadius() const;
	std::size_t getPointCount() const override;
	sf::Vector2f getPoint(std::size_t index) const override;

private:
	sf::Vector2f m_size;
	float m_radius;

private:
	static constexpr std::size_t corner_points = 16;
	static constexpr float pi = 3.141592654;
};