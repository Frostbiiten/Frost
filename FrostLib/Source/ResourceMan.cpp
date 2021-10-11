#include "ResourceMan.h"

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

		sf::Texture* getTexture(std::string path, std::string name, bool relative)
		{
			//Check if resource exists in map
			if (!textureMap.count(name))
			{
				std::string output;
				if (!AssetMan::readFile(name, output, relative, path)) throw std::invalid_argument("Unable to read \"" + name + "\"");
				std::unique_ptr<sf::Texture> tex = std::make_unique<sf::Texture>();
				if (!tex->loadFromMemory(output.c_str(), output.size())) throw std::runtime_error("Failed to load texture: invalid memory");
				textureMap.insert(std::make_pair(name, std::move(tex)));
			}

			return textureMap[name].get();
		}

		sf::SoundBuffer* getSound(std::string path, std::string name, bool relative)
		{
			//Check if resource exists in map
			if (!soundMap.count(name))
			{
				std::string output;
				if (!AssetMan::readFile(name, output, relative, path)) throw std::invalid_argument("Unable to read \"" + name + "\"");
				std::unique_ptr<sf::SoundBuffer> sound = std::make_unique<sf::SoundBuffer>();
				if (!sound->loadFromMemory(output.c_str(), output.size())) throw std::runtime_error("Failed to load sound: invalid memory");
				soundMap.insert(std::make_pair(name, std::move(sound)));
			}

			return soundMap[name].get();
		}

		sf::Music* getMusic(std::string path, std::string name, bool relative)
		{
			//Check if resource exists in map
			if (!musicMap.count(name))
			{
				std::string output;
				if (!AssetMan::readFile(name, output, relative, path)) throw std::invalid_argument("Unable to read \"" + name + "\"");
				std::unique_ptr<sf::Music> music = std::make_unique<sf::Music>();
				if (!music->openFromMemory(output.c_str(), output.size())) throw std::runtime_error("Failed to load music: invalid memory");
				musicMap.insert(std::make_pair(name, std::move(music)));
			}

			return musicMap[name].get();
		}

		void purgeResources()
		{
			textureMap.clear();
			soundMap.clear();
		}
	}
}
