#include <ApplicationManager.h>
#include <Debug.h>

void countLines()
{
	int totalLines = 0;
	bool inComment = false;

	//Count lines of code!!!
	for (auto const& dir_entry : std::filesystem::directory_iterator{ "Source" })
	{
		std::fstream fileStream;
		std::string name = dir_entry.path().string();
		fileStream.open(name);
		std::string line;
		int lineCount = 0;
		while (std::getline(fileStream, line))
		{
			if (line == "" ||
				line == "\n" ||
				line.rfind("//", 0) == 0
				) continue;
			if (line.rfind("/*") == 0)
			{
				inComment = true;
				continue;
			}

			if (line.find("*/") != std::string::npos)
			{
				inComment = false;
				continue;
			}

			if(!inComment) lineCount++;
		}

		totalLines += lineCount;
		//std::cout << name << ": " << lineCount << '\n';
		fl::Debug::log(name + ": " + std::to_string(lineCount));
	}
	fl::Debug::log("Total lines of code: " + std::to_string(totalLines));
}

int main()
{
	fl::ApplicationManager::init();
	return EXIT_SUCCESS;
}