#include <Scene.h>

#include <ApplicationManager.h>
#include <AssetMan.h>
#include <gameObject.h>
#include <Physics.h>
#include <UIElement.h>
#include <Utils.h>
#include <vector>

//Custom object types
#include <Player.h>

namespace fl
{
	scene::scene()
	{
		sceneName = uuids::to_string(uuids::uuid(uuids::uuid_system_generator{}()));
	}

	scene::scene(std::string name)
	{
		sceneName = name;
	}
	
	void scene::awake()
	{
		for (auto& element : gameObjects)
			if (element->active) element->awake();
	}
	
	void scene::start()
	{
		for (auto& element : gameObjects)
			if (element->active) element->start();
	}

	void scene::update()
	{
		for (auto& element : gameObjects)
			if (element->active) element->update();
	}

	void scene::fixedUpdate()
	{
		for (auto& element : gameObjects)
			if (element->active) element->preFixedUpdate();

		//Timestep is time since current and last fixedUpdate
		fl::Physics::step();

		for (auto& element : gameObjects)
			if (element->active) element->fixedUpdate();
	}

	void scene::render(sf::RenderWindow& window)
	{
		//Render UI Last
		for (auto& element : ui)
		{
			if(!element.parent)
				element.renderElement(window);
		}
	}

	void scene::invalidateUIDimensions()
	{
		for (auto& element : ui)
		{
			if (!element.parent && element.positionType != UI::PositionType::Constant && element.scalingType != UI::ScalingType::Constant)
			{
				element.invalidateDimensions();
			}
		}
	}

	gameObject* scene::createGameObject(nlohmann::json json, gameObject* parent)
	{
		std::string jsonType = json["type"];

		//Custom types can be defined here, they should be ordered from most common to rarest. i.e. collectibles (hundreds) to player (one)

		if (!parent)
		{
			if (jsonType == "player")
			{
				gameObjects.push_back(std::make_unique<Player>(json, this));
			}
			else if (jsonType == "basic")
			{
				gameObjects.push_back(std::make_unique<gameObject>(json, this));
			}
			else
				throw std::invalid_argument(Formatter() << "No appropriate constructor for object type \"" << jsonType << "\"");

			//Returns newly added object
			return gameObjects[gameObjects.size() - 1].get();
		}
		else
		{
			if (jsonType == "player")
			{
				parent->children.push_back(std::make_unique<Player>(json, this));
			}
			else if (jsonType == "basic")
			{
				parent->children.push_back(std::make_unique<gameObject>(json, this));
			}
			else
				throw std::invalid_argument(Formatter() << "No appropriate constructor for object type \"" << jsonType << "\"");

			//Returns newly added object
			return parent->children[gameObjects.size() - 1].get();
		}
	}

	bool scene::loadScene(std::string sceneName)
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

		//Construct all the scene elements : ADD OBJECT OVERRIDES
		for (nlohmann::json& element : json["gameObjects"])
		{
			//gameObjects.push_back(std::make_unique<gameObject>(element));
		}

		return true;
	}

	bool scene::saveScene(int jsonIndent)
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