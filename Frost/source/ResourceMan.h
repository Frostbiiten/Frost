#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <filesystem>

namespace fl
{
	namespace ResourceMan
	{
		sf::Texture* getTexture(std::filesystem::path file);
		sf::SoundBuffer* getSound(std::filesystem::path file);
		sf::Music* getMusic(std::filesystem::path file);

		void logResources();
		void purgeResources();
	}
}