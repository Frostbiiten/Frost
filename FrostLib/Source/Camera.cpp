#include <Camera.h>
#include <MathUtil.h>

namespace fl
{
	namespace Camera
	{
		sf::Vector2f scrollRange{ 8, 32 };
		sf::Vector2f clampedPlayerPosition;

		void updatePlayerCam(sf::Vector2f playerPosition)
		{
			clampedPlayerPosition = sf::Vector2f(
				Math::clamp(playerPosition.x - scrollRange.x, playerPosition.x + scrollRange.x, clampedPlayerPosition.x),
				Math::clamp(playerPosition.y - scrollRange.y, playerPosition.y + scrollRange.y, clampedPlayerPosition.y));

			cameraPosition = clampedPlayerPosition;

			cameraView.setCenter(Math::lerpVec(cameraView.getCenter(), cameraPosition, 0.5f));
			//cameraView.setCenter(cameraPosition);
			//cameraView.setSize(cameraView.getSize() * 1.005f);
			fl::ApplicationManager::getWindow()->setView(cameraView);
			fl::ApplicationManager::getBuffer()->setView(cameraView);
		}
	}
}
