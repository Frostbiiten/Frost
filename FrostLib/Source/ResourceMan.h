#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

namespace fl
{
	namespace ResourceMan
	{
		sf::Texture* getTexture(std::string path, std::string name, bool relative = false);
		sf::SoundBuffer* getSound(std::string path, std::string name, bool relative = false);
		void purgeResources();
	}
}