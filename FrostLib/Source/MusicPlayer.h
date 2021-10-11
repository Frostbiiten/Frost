#pragma once
#include <gameObject.h>
#include <Scene.h>
#include <SFML/Audio.hpp>

namespace fl
{
	namespace Audio
	{
		struct MusicPlayer : gameObject
		{
			std::string clipName; //Name of musicclip with file extension
			bool preLoad;
			bool loaded;
			bool common;
			bool autoPlay;
			bool loop;

			MusicPlayer(std::string clipName, bool preLoad = false, bool common = false, bool autoPlay = true, bool loop = true);
			MusicPlayer(nlohmann::json json);
			nlohmann::json serialize();

			void customInit();
			void play();
		};
	}
}
