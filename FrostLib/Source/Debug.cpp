#include <Debug.h>

#include <algorithm>
#include <ApplicationManager.h>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <time.h>

namespace fl
{
	void Debug::init()
	{
		struct tm newtime;
		time_t now = time(0);
		localtime_s(&newtime, &now);
		std::ostringstream dateStream;
		dateStream << "./Logs/" << std::put_time(&newtime, "%d-%m-%Y %H-%M-%S") << ".log";
		logName = dateStream.str();
		std::cout << logName << '\n';
		isInit = true;
		checkFolder();
	}

	void Debug::checkFolder()
	{
		if (!std::filesystem::exists("./Logs"))
		{
			std::filesystem::create_directory("./Logs");
			log("Created log folder because it did not exist.");
		}
	}

	std::string Debug::getType(const type_info& type)
	{
		if (type == typeid(std::string)) return "string";
		return type.name();
	}

	void Debug::drawCircle(sf::Vector2f position, float radius, float thickness, sf::Color color)
	{
		sf::CircleShape circle{radius};
		circle.setPosition(position);
		circle.setFillColor(sf::Color::Transparent);
		circle.setOutlineColor(color);
		circle.setOutlineThickness(thickness);
		ApplicationManager::getWindow()->draw(circle);
	}

	void Debug::drawRectangle(sf::Vector2f position, sf::Vector2f size, float rotation, float thickness, sf::Color color)
	{
		sf::RectangleShape rect{size};
		rect.setPosition(position);
		rect.setRotation(rotation);
		rect.setFillColor(sf::Color::Transparent);
		rect.setOutlineColor(color);
		rect.setOutlineThickness(thickness);
		ApplicationManager::getWindow()->draw(rect);
	}

	void Debug::drawLine(sf::Vector2f begin, sf::Vector2f end, sf::Color color)
	{
		sf::Vertex line[] = {sf::Vertex(begin), sf::Vertex(end)};
		line[0].color = color;
		line[1].color = color;
		ApplicationManager::getWindow()->draw(line, 2, sf::Lines);
	}

	void Debug::drawLineThick(sf::Vector2f begin, sf::Vector2f end, float thickness, sf::Color color)
	{
		sf::Vertex vertices[4];
		sf::Vector2f direction = end - begin;
		sf::Vector2f unitDirection = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
		sf::Vector2f unitPerpendicular(-unitDirection.y, unitDirection.x);

		sf::Vector2f offset = (thickness / 2.f) * unitPerpendicular;

		vertices[0].position = begin + offset;
		vertices[1].position = end + offset;
		vertices[2].position = begin - offset;
		vertices[3].position = end - offset;

		vertices[0].color = color;
		vertices[1].color = color;
		vertices[2].color = color;
		vertices[3].color = color;

		ApplicationManager::getWindow()->draw(vertices, 4, sf::Quads);
	}

	void Debug::drawRay(sf::Vector2f begin, sf::Vector2f direction, sf::Color color)
	{
		sf::Vertex line[] = {sf::Vertex(begin), sf::Vertex(direction)};
		line[0].color = color;
		line[1].color = color;
		ApplicationManager::getWindow()->draw(line, 2, sf::Lines);
	}

	void Debug::drawRayThick(sf::Vector2f begin, sf::Vector2f direction, float thickness, sf::Color color)
	{
		sf::Vertex vertices[4];
		sf::Vector2f unitDirection = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
		sf::Vector2f unitPerpendicular(-unitDirection.y, unitDirection.x);

		sf::Vector2f offset = (thickness / 2.f) * unitPerpendicular;

		vertices[0].position = begin + offset;
		vertices[1].position = begin + direction + offset;
		vertices[2].position = begin + direction - offset;
		vertices[3].position = begin - offset;

		vertices[0].color = color;
		vertices[1].color = color;
		vertices[2].color = color;
		vertices[3].color = color;

		ApplicationManager::getWindow()->draw(vertices, 4, sf::Quads);
	}
}
