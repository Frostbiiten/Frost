#include <ApplicationManager.h>

int main()
{
	int totalLines = 0;
	//Count lines of code!!!
	for (auto const& dir_entry : std::filesystem::directory_iterator{ "Source" })
	{
		//std::string data;
		//fl::AssetMan::readFile(dir_entry.path().string(), data);
		std::fstream fileStream;
		std::string name = dir_entry.path().string();
		fileStream.open(name);
		std::string line;
		int lineCount = 0;
		while (std::getline(fileStream, line))
		{
			if (line == "" || line == "\n") continue;
			lineCount++;
		}

		totalLines += lineCount;
		std::cout << name << ": " << lineCount << '\n';
	}
	std::cout << "______________________________\n";
	std::cout << "Total lines of code: " << totalLines << "\n";
	std::cout << "______________________________\n";
	fl::ApplicationManager::init();
	return EXIT_SUCCESS;
}