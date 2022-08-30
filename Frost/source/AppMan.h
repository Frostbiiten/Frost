#pragma once
#include <AssetMan.h>
#include <ResourceMan.h>
#include <Debug.h>
/*
#include <InputMan.h>
#include <Utils.h>
#include <Scene.h>
*/
#include <SFML/Graphics.hpp>

namespace fl
{
	namespace AppMan
	{
		extern sf::RenderWindow* windowPtr;

		// Appinfo
		static const std::string applicationName { "FL Prototype" };
		static const std::string developer { "Default Developer" };

		// The desired background color for the application
		static sf::Color backgroundColor{ 20, 20, 20 };

		// Pixel buffer resolution
		inline sf::Vector2f pixelSize{ 424, 240 };

		//Input
		//inline fl::InputMan::inputMap mainPlayer{ fl::InputMan::keyboardMap{} };
		//std::vector<inputMap> otherPlayers;

		//Base functions
		//Runs to start the application
		void Init();

		//Use with caution
		sf::RenderWindow* getWindow();
		//Use with caution
		sf::RenderTexture* getBuffer();
	}
}
