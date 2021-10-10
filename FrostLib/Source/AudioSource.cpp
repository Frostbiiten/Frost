#include "AudioSource.h"

#include <AssetMan.h>
#include <Debug.h>

namespace fl
{
	namespace Audio
	{

		AudioSource::AudioSource(nlohmann::json json)
		{
			name = json["name"];
			uuid = uuids::uuid::from_string(json["uuid"].get<std::string>());
			layer = json["layer"];
			active = json["active"];
			transform = deserializeTransform(json["transform"]);
			clipName = json["clipName"];
			preLoad = json["preLoad"];
			if (preLoad) loadAudio(*this);
			loaded = preLoad;
		}

		AudioSource::AudioSource(std::string clipName, bool preLoad)
		{
			uuid = uuids::uuid_random_generator{}();
			name = uuids::to_string(uuid);
			layer = Layer::Land;
			active = true;
			transform = sf::Transformable();
			this->clipName = clipName;
			this->preLoad = preLoad;
			if (preLoad) loadAudio(*this);
			loaded = preLoad;
		}

		nlohmann::json AudioSource::serialize()
		{
			nlohmann::json json = serializeBasic();
			json["transform"] = serializeTransform();
			json["type"] = "AudioSource";
			json["clipName"] = clipName;
			json["preLoad"] = preLoad;

			for (auto& child : children)
			{
				json["children"].push_back(child->serialize());
			}

			return json;
		}

		void AudioSource::play()
		{
			if (!loaded) loaded = loadAudio(*this);

			sound.play();
		}

		std::string audioFile;
		bool loadAudio(AudioSource& audioSource)
		{
			//If clip is not in memory, load it
			if (loadedAudio.count(audioSource.clipName) == 0)
			{
				loadedAudio.insert(std::make_pair(audioSource.clipName, sf::SoundBuffer()));

				//Find audiofile
				std::map<std::string, sf::SoundBuffer>::iterator it = loadedAudio.find(audioSource.clipName);
				sf::SoundBuffer buf;

				std::string filePath = "Scenes/" + audioSource.ownerScene->sceneName + "/Audio/";
				int fileSize = AssetMan::fileSize(audioSource.clipName, true, filePath);
				//audioFile.reserve(fileSize);
				//Buffer for audiofile
				if (AssetMan::readFile(audioSource.clipName, audioFile, true, filePath))
				{
					bool success = buf.loadFromMemory(&audioFile, fileSize);
				}
				else
				{
					Debug::log("Could not load audio file: \"" + audioSource.clipName + "\"");
					return false;
				}
			}

			std::map<std::string, sf::SoundBuffer>::iterator it = loadedAudio.find(audioSource.clipName);
			audioSource.sound.setBuffer(it->second);
			return true;
		}
	}
}
