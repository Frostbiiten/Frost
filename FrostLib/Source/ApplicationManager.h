#pragma once
#include <AssetMan.h>
#include <Debug.h>
#include <InputMan.h>
#include <Utils.h>
#include <Scene.h>
#include <SFML/Graphics.hpp>

extern sf::RenderWindow* windowPtr;

namespace fl
{
	//Forward declaration of scene
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
		inline constexpr int fixedTimestep = 16;
		inline float fps;

		//Input
		inline fl::InputMan::inputMap mainPlayer{ fl::InputMan::keyboardMap{} };
		//std::vector<inputMap> otherPlayers;

		//Base functions
		//Runs to start the application
		void init();

		//Runs every frame while the application is open
		void update();

		//Runs on frames based on a fixed step for scaling across different performances. Should mostly only be used for physics
		void fixedUpdate();

		void imGuiText(std::string text);

		//For non-pixel rendering
		void windowDrawElement(sf::Drawable& drawable);
		//For non-pixel window-relative rendering
		void windowDrawElementRelative(sf::Drawable& drawable);
		//For pixel rendering
		void bufferDrawElement(sf::Drawable& drawable);
		//For pixel window-relative rendering
		void bufferDrawElementRelative(sf::Drawable& drawable);
		
		//Use with caution
		sf::RenderWindow* getWindow();
		//Use with caution
		sf::RenderTexture* getBuffer();
	}
}
