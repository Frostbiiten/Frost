#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace fl
{
	//Options for logging data
	enum LogFlag
	{
		Type = 1 << 0,
		Time = 1 << 1,
		ContinueLine = 1 << 2
	};

	namespace Debug
	{
		//Debug log file stream
		inline std::fstream debugFile;

		//Debug log file name
		inline std::string logName;

		inline bool isInit;

		//Checks if log folder exists
		void checkFolder();

		//Converts type_info to readable strings
		std::string getType(const type_info& type);

		void init();

		//Appends message to log
		template<typename T>
		void log(const T& obj)
		{
			if (!isInit) init();
			debugFile.open(logName, std::ios::app);
			struct tm newtime;
			time_t now = time(0);
			localtime_s(&newtime, &now);
			debugFile << '[' << std::put_time(&newtime, "%H:%M:%S") << "] ";
			debugFile << obj << '\n';
			debugFile.close();
			std::cout << obj << '\n';
		}

		//Appends a message to log with specified flags
		template<typename T>
		void log(const T& obj, LogFlag flags)
		{
			if (!isInit) init();
			debugFile.open(logName, std::ios::app);

			//Time flag
			if (flags & LogFlag::Time == flags)
			{
				struct tm newtime;
				time_t now = time(0);
				localtime_s(&newtime, &now);
				debugFile << '[' << std::put_time(&newtime, "%H:%M:%S") << "] ";
			}

			//Type flag
			if (flags & LogFlag::Type == flags) debugFile << "[" << getType(typeid(obj)) << "] ";

			debugFile << obj;

			//Newline flag
			if (flags & LogFlag::ContinueLine != flags) debugFile << '\n';

			debugFile.close();

			std::cout << obj << '\n';
		}

		void drawCircle(sf::Vector2f position, float radius, float thickness, sf::Color color = sf::Color::White);
		void drawRectangle(sf::Vector2f position, sf::Vector2f size, float rotation, float thickness, sf::Color color = sf::Color::White);
		void drawLine(sf::Vector2f begin, sf::Vector2f end, sf::Color color = sf::Color::White);
		void drawLineThick(sf::Vector2f begin, sf::Vector2f end, float thickness, sf::Color color = sf::Color::White);
		void drawRay(sf::Vector2f begin, sf::Vector2f direction, sf::Color color = sf::Color::White);
		void drawRayThick(sf::Vector2f begin, sf::Vector2f direction, float thickness, sf::Color color = sf::Color::White);
	};
}
