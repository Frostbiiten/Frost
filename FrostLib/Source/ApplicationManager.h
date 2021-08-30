#pragma once
#include <AssetMan.h>
#include <Debug.h>
#include <InputMan.h>
#include <Utils.h>
#include <Scene.h>
#include <SFML/Graphics.hpp>

extern sf::RenderWindow* windowPtr;

//Forward declaration of scene class
namespace fl
{
	struct scene;

	namespace ApplicationManager
	{
		//Appinfo
		static const std::string applicationName{ "FL Prototype" };
		static const std::string developer{ "Default Developer" };

		//The desired background color for the application
		static sf::Color backgroundColor{ 20, 20, 20 };

		//Pixel resolution
		inline sf::Vector2f pixelSize{ 424, 240 };

		//Time: the timestep is in milliseconds
		inline constexpr int fixedTimestep = 20;
		inline float fps;

		//Input
		inline fl::InputMan::inputMap mainPlayer{ fl::InputMan::keyboardMap{} };
		//std::vector<inputMap> otherPlayers;

		//Base functions
		//Runs to start the application
		void init();

		//Runs before the window is created or any object-specific code is ran
		void awake();

		//Runs after the window is created. Object-specific code starts running
		void start();

		//Draws a scene to the window
		void drawScene(fl::scene scene);

		//Runs every frame while the application is open
		void update();

		//Runs on frames based on a fixed step for scaling across different performances. Should mostly only be used for physics
		void fixedUpdate();

		sf::RenderWindow* getWindow();
		sf::RenderTexture* getBuffer();
	}
}
