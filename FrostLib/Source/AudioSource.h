#pragma once
#include <gameObject.h>
#include <Scene.h>
#include <SFML/Audio.hpp>

namespace fl
{
	namespace Audio
	{
		struct AudioSource : gameObject
		{
			std::string clipName; //Name of audioclip with file extension
			sf::Sound sound;
			bool preLoad;
			bool loaded;
			bool common; //If the file is used across scenes

			AudioSource(std::string clipName, bool preLoad = false, bool common = true);
			AudioSource(nlohmann::json json);
			nlohmann::json serialize();

			void customInit();
			void play();
		};

		bool loadAudio(AudioSource& audioSource);
	}
}
