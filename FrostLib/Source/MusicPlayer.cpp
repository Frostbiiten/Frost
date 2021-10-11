#include <MusicPlayer.h>

#include <AssetMan.h>
#include <ResourceMan.h>
#include <Debug.h>

namespace fl
{
	namespace Audio
	{
		MusicPlayer::MusicPlayer(nlohmann::json json)
		{
			name = json["name"];
			uuid = uuids::uuid::from_string(json["uuid"].get<std::string>());
			layer = json["layer"];
			active = json["active"];
			transform = deserializeTransform(json["transform"]);
			clipName = json["clipName"];
			preLoad = json["preLoad"];
			loop = json["loop"];
			common = json["common"];
			autoPlay = json["autoPlay"];
			loaded = preLoad;
		}

		MusicPlayer::MusicPlayer(std::string clipName, bool preLoad, bool common, bool autoPlay, bool loop)
		{
			uuid = uuids::uuid_random_generator{}();
			name = uuids::to_string(uuid);
			layer = Layer::Land;
			active = true;
			transform = sf::Transformable();
			this->clipName = clipName;
			this->preLoad = preLoad;
			this->common = common;
			this->autoPlay = autoPlay;
			this->loop = loop;
			loaded = preLoad;
		}

		nlohmann::json MusicPlayer::serialize()
		{
			nlohmann::json json = serializeBasic();
			json["transform"] = serializeTransform();
			json["type"] = "MusicPlayer";
			json["clipName"] = clipName;
			json["preLoad"] = preLoad;
			json["common"] = common;
			json["autoPlay"] = autoPlay;
			json["loop"] = loop;

			for (auto& child : children)
			{
				json["children"].push_back(child->serialize());
			}

			return json;
		}

		void MusicPlayer::customInit()
		{
			if(preLoad)
			{
				if (common) ResourceMan::getMusic("Common/Audio/", clipName, true);
				else ResourceMan::getMusic("Scenes/" + ownerScene->sceneName + "/Audio/", clipName, true);		
			}	
			if (autoPlay) play();
		}

		void MusicPlayer::play()
		{
			auto music = ResourceMan::getMusic("Scenes/" + ownerScene->sceneName + "/Audio/", clipName, true);
			music->setLoop(loop);
			music->play();
		}
	}
}
