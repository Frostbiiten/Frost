#pragma once
#include <UIElement.h>
#include <vector>

namespace fl
{
	//Scenes are "worlds" that can be loaded and unloaded 
	//They are stored in directories with different files/folders for data
	struct Scene
	{
		std::string sceneName;
		std::vector<UI::UIElement> ui;

		//Constructor
		Scene();

		Scene(std::string name);

		//Render a scene to the display
		void render(sf::RenderWindow& window);

		//Invalidates the dimensions of ui for screen scaling
		void invalidateUIDimensions();

		//Load a scene from json representation
		bool loadScene(std::string sceneName);

		//Saves a scene in json representation
		bool saveScene(int jsonIndent = -1);
	};
}
