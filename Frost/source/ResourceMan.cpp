#include <ResourceMan.h>

#include <Debug.h>
#include <AssetMan.h>
#include <iostream>
#include <map>

namespace fl
{
	namespace ResourceMan
	{
		//Maps for all the loaded resources
		std::map<std::string, std::unique_ptr<sf::Texture>> textureMap;
		std::map<std::string, std::unique_ptr<sf::SoundBuffer>> soundMap;
		std::map<std::string, std::unique_ptr<sf::Music>> musicMap;
		std::map<sf::Music*, std::string> musicMemoryMap;

		sf::Texture* getTexture(std::filesystem::path file)
		{
			//Check if resource exists in map
			if (!textureMap.count(file.filename().string()))
			{
				std::string output;
				if (!AssetMan::readFile(file, output)) throw std::invalid_argument(fmt::format("Unable to read \"{}\"", file.filename().string()));
				std::unique_ptr<sf::Texture> tex = std::make_unique<sf::Texture>();
				if (!tex->loadFromMemory(output.c_str(), output.size())) throw std::runtime_error("Failed to load texture: invalid memory");
				textureMap.insert(std::make_pair(file.filename().string(), std::move(tex)));
			}

			return textureMap[file.filename().string()].get();
		}
		sf::SoundBuffer* getSound(std::filesystem::path file)
		{
			//Check if resource exists in map
			if (!soundMap.count(file.filename().string()))
			{
				std::string output;
				if (!AssetMan::readFile(file, output)) throw std::invalid_argument(fmt::format("Unable to read \"{}\"", file.filename().string()));
				std::unique_ptr<sf::SoundBuffer> sound = std::make_unique<sf::SoundBuffer>();
				if (!sound->loadFromMemory(output.c_str(), output.size())) throw std::runtime_error("Failed to load sound: invalid memory");
				soundMap.insert(std::make_pair(file.filename().string(), std::move(sound)));
			}

			return soundMap[file.filename().string()].get();
		}
		sf::Music* getMusic(std::filesystem::path file)
		{
			//Check if resource exists in map
			if (!musicMap.count(file.filename().string()))
			{
				std::string output;
				if (!AssetMan::readFile(file, output)) throw std::invalid_argument(fmt::format("Unable to read \"{}\"", file.filename().string()));
				std::unique_ptr<sf::Music> music = std::make_unique<sf::Music>();
				musicMemoryMap.insert(std::make_pair(music.get(), std::move(output)));
				if (!music->openFromMemory(musicMemoryMap[music.get()].c_str(), musicMemoryMap[music.get()].size())) throw std::runtime_error("Failed to load music: invalid memory");
				musicMap.insert(std::make_pair(file.filename().string(), std::move(music)));
			}

			return musicMap[file.filename().string()].get();
		}

		void logResources()
		{
			Debug::log()->info("{} Textures loaded:", textureMap.size());
			for (auto& entry : textureMap) Debug::log()->info("\t {}", entry.first);

			Debug::log()->info("{} Sounds loaded:", soundMap.size());
			for (auto& entry : soundMap) Debug::log()->info("\t {}", entry.first);

			Debug::log()->info("{} Music tracks loaded:", musicMap.size());
			for (auto& entry : musicMap) Debug::log()->info("\t {} {} bytes", entry.first, musicMemoryMap[entry.second.get()].size());
		}
		void purgeResources()
		{
			logResources();
			std::size_t texCount = textureMap.size();
			std::size_t soundCount = soundMap.size();
			std::size_t musicCount = musicMap.size();
			for (auto& song : musicMap)
			{
				song.second->stop();
			}
			textureMap.clear();
			soundMap.clear();
			musicMap.clear();
			musicMemoryMap.clear();

			Debug::log()->info("Cleared {} textures", texCount);
			Debug::log()->info("Cleared {} sounds", soundCount);
			Debug::log()->info("Cleared {} music tracks", musicCount);
		}
	}
}
