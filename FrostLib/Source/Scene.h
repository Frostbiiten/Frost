#pragma once
#include <UIElement.h>
#include <gameObject.h>
#include <vector>

namespace fl
{
	//Scenes are "worlds" that can be loaded and unloaded 
	//They are stored in directories with different files/folders for data

	//Forward declaration
	struct gameObject;

	struct scene
	{
		std::vector<std::unique_ptr<gameObject>> gameObjects;
		//TODO: Deprecate this
		std::vector<UI::UIElement> ui;

		std::string sceneName;

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

		//Render a scene to the display : Deprecated
		void render(sf::RenderWindow& window);

		//Invalidates the dimensions of ui for screen scaling
		void invalidateUIDimensions();

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
