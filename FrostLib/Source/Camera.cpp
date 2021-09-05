#include <Camera.h>
#include <MathUtil.h>

namespace fl
{
	namespace Camera
	{
		sf::Vector2f scrollRange{ 8, 32 };

		void updatePlayerCam(sf::Vector2f playerPosition)
		{
			//The difference between the camera position and the player
			sf::Vector2f diff = playerPosition - cameraPosition;

			//X-Range
			if (diff.x < -scrollRange.x || diff.x > scrollRange.x)
				cameraPosition.x += diff.x;

			//Y-Range
			if (diff.y < -scrollRange.y || diff.y > scrollRange.y)
				cameraPosition.y += diff.y;

			cameraView.setCenter(Math::lerpVec(cameraView.getCenter(), cameraPosition, 0.1f));
			cameraView.setSize(cameraView.getSize() * 1.003f);
			cameraView.setRotation(cameraView.getRotation() + 0.1f);
			fl::ApplicationManager::getWindow()->setView(cameraView);
			fl::ApplicationManager::getBuffer()->setView(cameraView);
		}
	}
}
