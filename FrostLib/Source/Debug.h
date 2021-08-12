#pragma once
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <SFML/Graphics.hpp>

namespace FrostLib
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
		namespace
		{
			//Debug log file stream
			std::fstream debugFile;

			//Debug log file name
			std::string logName;

			bool isInit;
		}

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
		}
	};
}
