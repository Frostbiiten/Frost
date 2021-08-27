#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/View.hpp>
#include <ApplicationManager.h>

namespace fl
{
	namespace Camera
	{
		inline sf::Vector2f cameraPosition;
		inline sf::View cameraView{ sf::Vector2f(), fl::ApplicationManager::pixelSize };
		void updatePlayerCam(sf::Vector2f playerPosition);
	}
}
