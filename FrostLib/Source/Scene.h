#pragma once
#include <gameObject.h>
#include <json.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>

namespace fl
{
	//Scenes are "worlds" that can be loaded and unloaded 
	//They are stored in directories with different files/folders for data

	//Forward declaration
	struct gameObject;

	struct scene
	{
		std::string sceneName;

		//Top-layer objects
		std::vector<std::unique_ptr<gameObject>> gameObjects;

		//Constructor
		scene();
		scene(std::string name);

		//Scenes should be non-copyable, pass by reference instead
		scene(const scene&) = delete;
		scene& operator=(const scene&) = delete;

		void awake();
		void start();
		void update();
		void fixedUpdate();

		/// Creates a gameObject using the specified json
		/// Overrides are specified here
		/// @param The json representation of the gameObject
		/// @return Returns a reference to the unique pointer of the gameObject created in the vector
		gameObject* createGameObject(nlohmann::json json, gameObject* parent = nullptr);

		//Load a scene from json representation
		bool loadScene(std::string sceneName);

		//Saves a scene in json representation
		bool saveScene(int jsonIndent = -1);
	};
}
