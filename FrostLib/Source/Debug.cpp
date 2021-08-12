#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <time.h>
#include <Debug.h>

namespace FrostLib
{
	void Debug::init()
	{
		struct tm newtime;
		time_t now = time(0);
		localtime_s(&newtime, &now);
		std::ostringstream dateStream;
		dateStream << "./Logs/" << std::put_time(&newtime, "%d-%m-%Y %H-%M-%S") << ".log";
		logName = dateStream.str();
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
}
