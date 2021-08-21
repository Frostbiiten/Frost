#pragma once
#include <UIElement.h>
#include <gameObject.h>
#include <vector>

namespace fl
{
	//Scenes are "worlds" that can be loaded and unloaded 
	//They are stored in directories with different files/folders for data
	struct Scene
	{
		std::string sceneName;
		//TODO: Deprecate this
		std::vector<UI::UIElement> ui;
		std::vector<std::shared_ptr<gameObject>> gameObjects;

		//Constructor
		Scene();

		Scene(std::string name);

		void awake();
		void start();
		void update();
		void fixedUpdate();

		//Render a scene to the display : Deprecated
		void render(sf::RenderWindow& window);

		//Invalidates the dimensions of ui for screen scaling
		void invalidateUIDimensions();

		//Load a scene from json representation
		bool loadScene(std::string sceneName);

		//Saves a scene in json representation
		bool saveScene(int jsonIndent = -1);
	};
}
