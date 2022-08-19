#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <iostream>

// spdlog
#include <spdlog/async.h>

namespace fl
{
	namespace Debug
	{
		// Init logs
		void init();

		//Checks if log folder exists
		void checkFolder();

		//Converts type_info to readable strings
		std::string getType(const type_info& type);

		// Get Log to log message
		spdlog::logger* log();

		// Visual debug
		void drawCircle(sf::Vector2f position, float radius, float thickness, sf::Color color = sf::Color::White);
		void drawRectangle(sf::Vector2f position, sf::Vector2f size, float rotation, float thickness, sf::Color color = sf::Color::White);
		void drawLine(sf::Vector2f begin, sf::Vector2f end, sf::Color color = sf::Color::White);
		void drawLineThick(sf::Vector2f begin, sf::Vector2f end, float thickness, sf::Color color = sf::Color::White);
		void drawRay(sf::Vector2f begin, sf::Vector2f direction, sf::Color color = sf::Color::White);
		void drawRayThick(sf::Vector2f begin, sf::Vector2f direction, float thickness, sf::Color color = sf::Color::White);
		void dumpPhysicsWorld();
	};
}
