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

		// Texture IDs
		sf::Texture* getIDTexture(unsigned int id); // Gets the corresponding texture from an id. ONLY TO BE USED WHEN DESERIALIZING; ids are just created when serializing
		std::pair<unsigned int, std::filesystem::path> getTextureID(sf::Texture* texture); // Gets the corresponding id-path pair from a texture
		std::map<unsigned int, std::filesystem::path> getTextureIDMap();
		void loadTextureIDs(std::map<unsigned int, std::filesystem::path> map, bool additive);
	}
}