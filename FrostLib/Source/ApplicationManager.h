#pragma once
#include <AssetMan.h>
#include <Debug.h>
#include <Utils.h>
#include <SFML/Graphics.hpp>
#include <Scene.h>
#include <WindowWrapper.h>

extern sf::RenderWindow* windowPtr;

namespace ApplicationManager
{
	//Appinfo
	static const std::string applicationName{ "FL Prototype" };
	static const std::string developer{ "Default Developer" };

	//The desired background color for the application
	static sf::Color backgroundColor{ 20, 20, 20 };

	//Base functions
	//Runs to start the application
	void init();

	//Runs before the window is created or any object-specific code is ran
	void awake();

	//Runs after the window is created. Object-specific code starts running
	void start();

	//Draws a scene to the window
	void drawScene(FrostLib::Scene scene);

	//Runs every frame while the application is open
	void update();
}
