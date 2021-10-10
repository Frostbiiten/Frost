#pragma once
#include <gameObject.h>
#include <Scene.h>
#include <SFML/Audio.hpp>

namespace fl
{
	namespace Audio
	{
		//Global audio memory buffer
		inline std::map<std::string, sf::SoundBuffer> loadedAudio;

		struct AudioSource : gameObject
		{
			std::string clipName; //Name of audiocliip with file extension
			sf::Sound sound;
			bool loaded;
			bool preLoad;

			AudioSource(std::string clipName, bool preLoad = false);
			AudioSource(nlohmann::json json);
			nlohmann::json serialize();

			void play();
		};

		bool loadAudio(AudioSource& audioSource);
	}
}
