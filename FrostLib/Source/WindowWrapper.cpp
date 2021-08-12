#include "WindowWrapper.h"

FrostLib::WindowWrapper::WindowWrapper()
{
	//any setup
}

void FrostLib::WindowWrapper::init(sf::VideoMode mode, const std::string& title, sf::Uint32 style, sf::ContextSettings contextSettings)
{
	window.create(mode, title, style, contextSettings);
}

sf::RenderWindow& FrostLib::WindowWrapper::getWindow()
{
	return window;
}

void FrostLib::WindowWrapper::draw(const sf::Drawable& drawable, const sf::RenderStates& renderstate)
{
	window.draw(drawable, renderstate);
}
