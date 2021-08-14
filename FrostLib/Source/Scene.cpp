#include <Scene.h>

#include <ApplicationManager.h>
#include <AssetMan.h>
#include <UIElement.h>
#include <Utils.h>
#include <vector>

namespace fl
{
	Scene::Scene()
	{
		sceneName = uuids::to_string(uuids::uuid(uuids::uuid_system_generator{}()));
	}

	Scene::Scene(std::string name)
	{
		sceneName = name;
	}

	void Scene::render(sf::RenderWindow& window)
	{
		//Render UI Last
		for (auto& element : ui)
		{
			if(!element.parent)
				element.renderElement(window);
		}
	}

	void Scene::invalidateUIDimensions()
	{
		for (auto& element : ui)
		{
			if (!element.parent && element.positionType != UI::PositionType::Constant && element.scalingType != UI::ScalingType::Constant)
			{
				element.invalidateDimensions();
			}
		}
	}

	bool Scene::loadScene(std::string sceneName)
	{
		//Json representation
		nlohmann::json json;

		//Load string from scene file
		std::string output;
		fl::AssetMan::readFile(sceneName + ".json", output, true, "Scenes/" + sceneName + '/');

		//Parse json
		try
		{
			json = nlohmann::json::parse(output);
		}
		catch (nlohmann::json::exception err)
		{
			fl::Debug::log("JSON ERROR " + err.id + std::string(": ") + err.what());
			return false;
		}

		//Construct all the ui elements
		for (nlohmann::json& element : json["UI"])
		{
			try
			{
				ui.push_back(UI::UIElement(element, ui));
			}
			catch (nlohmann::json::type_error& err)
			{	
				fl::Debug::log("JSON TYPE ERROR " + err.id + std::string(": ") + err.what());
			}
		}

		return true;
	}

	bool Scene::saveScene(int jsonIndent)
	{
		//Becuse writing to archives is currently not possible, it will be saved to a folder
		nlohmann::json finalJson;
		
		//Creates scene folder
		if(!fl::AssetMan::exists("Scenes"))
			fl::AssetMan::createDirectory("Scenes");
		std::string folderName = "Scenes/" + sceneName;
		if(!fl::AssetMan::exists(folderName))
			fl::AssetMan::createDirectory(folderName);

		//UI
		{
			nlohmann::json uiJson;
			for (size_t x = 0; x < ui.size(); x++)
			{
				//If the object has a parent, it will already be serialized - so skip over
				if (ui[x].parent) continue;
				uiJson.push_back(ui[x].serialize());
			}

			finalJson["UI"] = uiJson;
		}

		//Writes json
		fl::AssetMan::writeFile(sceneName + ".json", finalJson.dump(jsonIndent), false, true, "Scenes/" + sceneName + '/');

		return true;
	}
}