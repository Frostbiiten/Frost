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
			bool loaded;
			bool preLoad;

			AudioSource(std::string clipName, bool preLoad = false);
			AudioSource(nlohmann::json json);
			nlohmann::json serialize();

			void awake();

			void play();
		};

		bool loadAudio(AudioSource& audioSource);
	}
}
