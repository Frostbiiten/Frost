#include <AssetMan.h>

#include <Debug.h>
#include <physfs.h>
#include <filesystem>
#include <fstream>
#include <iostream>


bool FrostLib::AssetMan::init()
{
	if (PHYSFS_isInit()) return false;
	if (!PHYSFS_init(NULL)) arc_logLatestError();
	PHYSFS_permitSymbolicLinks(1);

	//Mount all archives in subdirectories.
	for (const auto& entry : std::filesystem::recursive_directory_iterator("./"))
	{
		if (entry.path().extension() == ".zip" ||
			entry.path().extension() == ".7z" ||
			entry.path().extension() == ".pak" ||
			entry.path().extension() == ".pkg")
		{
			std::cout << "Mounting " << entry.path().string() << '\n';
			arc_mountDir(entry.path().string());
		}
	}

	isInit = true;
	return true;
}

void FrostLib::AssetMan::arc_logLatestError()
{
	if (!isInit) init();
	PHYSFS_ErrorCode err = PHYSFS_getLastErrorCode();
	std::string errText = PHYSFS_getErrorByCode(err);
	std::string logText = "PHYSFS FILESYSTEM ERROR " + std::to_string(err) + ": " + errText;
	FrostLib::Debug::log(logText);
	std::cout << logText << '\n';
}

void FrostLib::AssetMan::fs_logError(std::error_code err)
{
	if (!isInit) init();

	std::string logText = "STANDARD FILESYSTEM ERROR " + std::to_string(err.value()) + ": " + err.message();
	FrostLib::Debug::log(logText);
	std::cout << logText << '\n';
}

bool FrostLib::AssetMan::arc_mountDir(std::string dir, std::string mountPoint)
{
	if (!isInit) init();

	//runs and returns 0 on fail
	if (!PHYSFS_mount(dir.c_str(), mountPoint.c_str(), 1))
	{
		arc_logLatestError();
		return false;
	}
	else return true;
}

bool FrostLib::AssetMan::arc_unmountDir(std::string dir)
{
	if (!isInit) init();

	//runs and returns 0 on fail
	if (!PHYSFS_unmount(dir.c_str()))
	{
		arc_logLatestError();
		return false;
	}
	else return true;
}

std::vector<std::string> FrostLib::AssetMan::list(std::string dir, bool relative)
{
	if (!isInit) init();

	std::vector<std::string> list;

	std::string dirPath;
	if (relative)
		dirPath = "./" + dir;
	else
		dirPath = dir;

	//STD::FILESYSTEM SECTION

	//Iterates in the specified dierectory and adds files to list
	for (const auto& entry : std::filesystem::directory_iterator(dirPath))
		list.push_back(entry.path().string());

	//If it is not relative, return list because physfs is not needed
	if (!relative) return list;


	//PHYSFS SECTION

	//returns a NULL terminated array of NULL terminated strings
	char** listBuf = PHYSFS_enumerateFiles(dir.c_str());

	//returns empty list if the buffer is NULL
	if (listBuf == NULL) return list;

	//Iterates through each element in the array, stops at NULL because it is NULL terminated
	for (size_t x = 0; listBuf[x] != NULL; x++)
	{
		//Adds constructed NULL terminated strings to the vector
		list.push_back(std::string(listBuf[x], listBuf[x] + strlen(listBuf[x])));
	}

	//returns filled list
	return list;
}

bool FrostLib::AssetMan::exists(std::string file)
{
	if (!isInit) init();

	//mounts current directory and unmounts after checking for existence
	arc_mountDir("./");
	bool exists = PHYSFS_exists(file.c_str());
	arc_unmountDir("./");
	return exists;
}

std::string FrostLib::AssetMan::getBaseDirectory()
{
	return std::filesystem::current_path().string();
}

bool FrostLib::AssetMan::readFile(std::string fileName, std::string& output, bool relative, std::string path)
{
	if (!isInit) init();

	std::string filePath;
	if (relative)
		filePath = "./" + path + fileName;
	else
		filePath = path + fileName;

	//STD::FILESYSTEM SECTION

	//Only run std::filesytem code if it exists in the current path and is a normal file
	if (std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath))
	{
		output.clear();
		std::ifstream stream(filePath);
		if (!stream.is_open()) return false;
		output = std::string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
		return true;
	}

	//PHYSFS SECTION

	//Removes ./ for physfs
	std::string PHYSFSPath = filePath.substr(2, filePath.size() - 2);
	PHYSFS_file* fileData = PHYSFS_openRead(PHYSFSPath.c_str());

	if (fileData != nullptr)
	{
		//Gets filesize
		PHYSFS_sint64 fileSize = PHYSFS_fileLength(fileData);

		//If the size is -1, return failure
		if (fileSize == -1) return false;

		//Creates a byte/char array to store data inside, + 1 byte for it to be terminated 
		char* buffer = new char[fileSize + 1];

		//Attempts to read data into buffer
		PHYSFS_sint64 dataRead = PHYSFS_readBytes(fileData, buffer, fileSize);

		//Set the terminator
		buffer[fileSize] = '\0';

		if (dataRead != fileSize)
		{
			//Data has not been read properly, clear buffer memory
			delete[] buffer;
			arc_logLatestError();
			return false;
		}
		else
		{
			//Copies buffer into output
			output = std::string(buffer);
			//clear buffer memory
			delete[] buffer;
			return true;
		}

		//Attempt to close file handle
		if (!PHYSFS_close(fileData)) arc_logLatestError();
	}
	else arc_logLatestError();

	//defults to failiure
	return false;
}

bool FrostLib::AssetMan::createDirectory(std::string path, bool relative)
{
	std::string dirPath;
	if (relative)
		dirPath = "./" + path;
	else
		dirPath = path;

	std::error_code err;
	bool success = std::filesystem::create_directory(dirPath, err);

	if (success) return success;
	
	fs_logError(err);

	return false;
}

bool FrostLib::AssetMan::removeDirectory(std::string path, bool relative)
{
	std::string dirPath;
	if (relative)
		dirPath = "./" + path;
	else
		dirPath = path;

	//Does not exist
	if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath))
		return false;

	std::error_code err;
	bool success = std::filesystem::remove_all(dirPath, err);

	if (success) return success;

	fs_logError(err);

	return false;
}

bool FrostLib::AssetMan::createFile(std::string fileName, bool relative, std::string path)
{
	std::string filePath;
	if (relative)
		filePath = "./" + path + fileName;
	else
		filePath = path + fileName;

	std::ofstream file{ filePath };
	if (std::filesystem::exists(filePath)) return true;

	return false;
}

bool FrostLib::AssetMan::removeFile(std::string fileName, bool relative, std::string path)
{
	std::string filePath;
	if (relative)
		filePath = "./" + path + fileName;
	else
		filePath = path + fileName;

	//Does not exist or not a regular file
	if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath))
		return false;

	std::error_code err;
	bool success = std::filesystem::remove(filePath, err);

	if (success) return success;

	fs_logError(err);

	return false;
}

bool FrostLib::AssetMan::writeFile(std::string fileName, std::string data, bool append, bool relative, std::string path)
{
	std::string filePath;
	if (relative)
		filePath = "./" + path + fileName;
	else
		filePath = path + fileName;

	/*
	Files should be able to be written without creating the file firstly
	if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath))
		return false;
	*/

	std::fstream fileStream;
	if (append)
		fileStream.open(filePath, std::ofstream::out | std::ios::app);
	else
		fileStream.open(filePath, std::ofstream::out | std::ofstream::trunc);

	if (!fileStream.is_open()) return false;

	fileStream << data;
	return true;
}
