#pragma once
#include <SFML/Graphics.hpp>

namespace FrostLib
{
	class WindowWrapper
	{
		sf::RenderWindow window;

	public:
		WindowWrapper();

		void init(sf::VideoMode mode = sf::VideoMode(600, 600), const std::string& title = "", sf::Uint32 style = 7U, sf::ContextSettings contextSettings = sf::ContextSettings());

		sf::RenderWindow& getWindow();

		void draw(const sf::Drawable& drawable, const sf::RenderStates& renderstate = sf::RenderStates::Default);
	};
}
