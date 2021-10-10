#include <Scene.h>

#include <ApplicationManager.h>
#include <AssetMan.h>
#include <gameObject.h>
#include <Physics.h>
#include <Utils.h>
#include <vector>

//Custom object types
#include <Player.h>
#include <SplineTerrain.h>

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
		/*
		gameObjects.push_back(std::make_unique<SplineTerrain>(this));
		SplineTerrain* terrainPtr = dynamic_cast<SplineTerrain*>(gameObjects[gameObjects.size() - 1].get());
		terrainPtr->spline.addNode(sf::Vector2f(0.f, 100.f), sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 0.f), 0);
		terrainPtr->spline.addNode(sf::Vector2f(50.f, 0.f), sf::Vector2f(-50.f, 0.f), sf::Vector2f(50.f, 0.f), 1);
		terrainPtr->spline.addNode(sf::Vector2f(100.f, 100.f), sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 0.f), 2);
		terrainPtr->generateOutline();
		terrainPtr->simplifyOutline();
		terrainPtr->generateShape();
		terrainPtr->generateCollider();
		*/
		//saveScene(4);
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

	gameObject* scene::createGameObject(nlohmann::json json, gameObject* parent)
	{
		std::string jsonType = json["type"];

		std::vector<std::unique_ptr<gameObject>>* objectVector = parent ? &parent->children : &gameObjects;

		if (jsonType == "terrain")
		{
			objectVector->push_back(std::make_unique<SplineTerrain>(json));
		}
		else if (jsonType == "player")
		{
			objectVector->push_back(std::make_unique<Player>(json));
		}
		else if (jsonType == "basic")
		{
			objectVector->push_back(std::make_unique<gameObject>(json));
		}
		else
			throw std::invalid_argument(Formatter() << "No appropriate constructor for object type \"" << jsonType << "\"");

		//Returns newly added object
		gameObject* obj = gameObjects[gameObjects.size() - 1].get();
		obj->ownerScene = this;

		for (auto& child : json["children"])
		{
			createGameObject(child, obj);
		}

		//Returns newly added object
		return obj;
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

		//Construct all the scene elements
		gameObjects.reserve(json["gameObjects"].size());
		for (nlohmann::json& element : json["gameObjects"])
		{
			createGameObject(element);
		}

		return true;
	}

	void scene::clearScene()
	{
		gameObjects.clear();
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

		//Gameobjects
		for (auto& obj : gameObjects)
		{
			finalJson["gameObjects"].push_back(obj.get()->serialize());
		}

		//Writes json
		fl::AssetMan::writeFile(sceneName + ".json", finalJson.dump(jsonIndent), false, true, "Scenes/" + sceneName + '/');

		return true;
	}
}