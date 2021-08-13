#pragma once
#include <iostream>
#include <vector>

namespace FrostLib
{
	//Unified* io class
	namespace AssetMan
	{
		inline bool isInit;

		//Initiates the basics 
		bool init();

		//Logs the latest PHYSFS error that occured
		void arc_logLatestError();

		//Logs a std::filesystem error
		void fs_logError(std::error_code err);

		//Mounts an archive to the virtual filesystem. "dir" is the directory of the archive. "mountpoint" is where it will be "mounted"/added on the virtual filesystem
		bool arc_mountDir(std::string dir, std::string mountPoint = "");

		//Unmounts an archive to the virtual filesystem. "dir" is the directory of the archive.
		bool arc_unmountDir(std::string dir);

		//UNIFIED

		//Returns a vector of string file names from directory "dir"
		std::vector<std::string> list(std::string dir, bool relative = true);

		//Check if "file" exists in the filesystem. Returns true if found, else false.
		bool exists(std::string file);

		//Returns the directory that the application was launched from
		std::string getBaseDirectory();

		//Loads data from file into string - prefers std::filesystem over
		bool readFile(std::string fileName, std::string& output, bool relative = false, std::string path = "");

		//STD::FILESYSTEM ONLY (No writing to archive support)

		//Creates a directory
		bool createDirectory(std::string path, bool relative = true);

		//Removes a directory
		bool removeDirectory(std::string path, bool relative = true);

		//Creates a file
		bool createFile(std::string fileName, bool relative = true, std::string path = "");

		//Removes a file
		bool removeFile(std::string fileName, bool relative = true, std::string path = "");

		//Writes data from string into a file
		bool writeFile(std::string fileName, std::string data, bool append = false, bool relative = true, std::string path = "");
	};
}
