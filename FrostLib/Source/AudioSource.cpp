#include "AudioSource.h"

#include <AssetMan.h>
#include <ResourceMan.h>
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

		void AudioSource::awake()
		{
			play();
		}

		void AudioSource::play()
		{
			if (!loaded) loaded = loadAudio(*this);
			sound.play();
		}

		bool loadAudio(AudioSource& audioSource)
		{
			try
			{
				audioSource.sound.setBuffer(*ResourceMan::getSound("Scenes/" + audioSource.ownerScene->sceneName + "/Audio/", audioSource.clipName, true));
			}
			catch (std::runtime_error err)
			{
				Debug::log(std::string("Audio loading runtime error: ") + err.what());
				return false;
			}
			return true;
		}
	}
}
