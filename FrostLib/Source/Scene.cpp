#include <ApplicationManager.h>
#include <AssetMan.h>
#include <UIElement.h>
#include <Utils.h>
#include <Scene.h>
#include <vector>

namespace FrostLib
{
	Scene::Scene()
	{
		//any custom setup...	
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
		FrostLib::AssetMan::readFile("Scenes/" + sceneName + ".json", output);
		
		//Parse json
		try
		{
			json.parse(output);
		}
		catch (nlohmann::json::exception err)
		{
			FrostLib::Debug::log("JSON ERROR " + err.id + std::string(": ") + err.what());
			return false;
		}


		//Construct all the ui elements
		for (nlohmann::json& element : json["UI"])
		{
			ui.push_back(std::move(UI::UIElement(element, ui)));
		}

		return true;
	}

	bool Scene::saveScene(int jsonIndent)
	{
		//Becuse writing to archives is currently not possible, it will be saved to a folder
		
		nlohmann::json finalJson;
		
		//Creates scene folder
		FrostLib::AssetMan::createDirectory("Scenes");
		FrostLib::AssetMan::createDirectory("Scenes/" + sceneName);

		//UI
		for (size_t x = 0; x < ui.size(); x++)
		{
			//If the object has a parent, it will already be serialized - so skip over
			if (ui[x].parent) continue;
			finalJson["UI"][uuids::to_string(ui[x].uuid)] = ui[x].serialize();
		}

		//Writes json
		FrostLib::AssetMan::writeFile(sceneName + ".json", finalJson.dump(), false, true, "./Scenes/" + sceneName);

		return true;
	}
}