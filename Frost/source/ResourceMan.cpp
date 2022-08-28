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

		// Map for texture (de)serialization
		std::map<sf::Texture*, std::pair<unsigned int, std::filesystem::path>> textureIDs;

		// For deserialization (I believe a bimap implementation here would be overkill)
		// int and pointer already use a small amount of space so an additional pointer wouldn't make sense
		std::map<unsigned int, sf::Texture*> reverseTextureIDs;

		sf::Texture* getTexture(std::filesystem::path file)
		{
			sf::Texture* texPtr;

			//Check if resource exists in map
			if (!textureMap.count(file.filename().string()))
			{
				std::string output;
				if (!AssetMan::readFile(file, output)) throw std::invalid_argument(fmt::format("Unable to read \"{}\"", file.filename().string())); // Read data from file to string output
				std::unique_ptr<sf::Texture> tex = std::make_unique<sf::Texture>(); // Creates a instance of a pointer
				if (!tex->loadFromMemory(output.c_str(), output.size())) throw std::runtime_error("Failed to load texture: invalid memory"); // Attempt to load from memory
				texPtr = textureMap.insert(std::make_pair(file.filename().string(), std::move(tex))).first->second.get(); // Insert and retrieve pointer to texture

				// Update textureIDs because path never existed in texture map
				textureIDs[texPtr] = std::make_pair(textureIDs.size(), file);
			}
			else
			{
				texPtr = textureMap[file.filename().string()].get();
			}

			return texPtr;
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

		sf::Texture* getIDTexture(unsigned int id)
		{
			return reverseTextureIDs[id];
		}
		std::pair<unsigned int, std::filesystem::path> getTextureID(sf::Texture* texture)
		{
			std::map<sf::Texture*, std::pair<unsigned int, std::filesystem::path>>::iterator id = textureIDs.find(texture);
			if (id != textureIDs.end())
			{
				// Found, return id
				return id->second;
			}
			else
			{
				// Not found, this should't be possible. Possibly return 'common/null' ?
				Debug::log()->error("Attempting to get ID of an invalid texture!");
				throw std::runtime_error("Attempting to get ID of an invalid texture!");
			}
		}
		std::map<unsigned int, std::filesystem::path> getTextureIDMap()
		{
			// Don't return texture pointers because they will be invalidated anyways
			std::map<unsigned int, std::filesystem::path> map;

			// Create map with int-path pairs
			for (auto const& m : textureIDs)
				map.insert(std::make_pair(m.second.first, m.second.second));

			return map;
		}
		void loadTextureIDs(std::map<unsigned int, std::filesystem::path> map, bool additive)
		{
			for (auto& pair : map)
			{
				// Sets the texture id based on pointer to int-path pair
				// In the process of retrieving the pointer, it inevitably calls getTexture to load the corresponding texture
				sf::Texture* ptr = getTexture(pair.second);
				textureIDs[ptr] = pair;
				reverseTextureIDs[pair.first] = ptr;
			}
		}
	}
}
