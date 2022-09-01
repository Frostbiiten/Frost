#include <EditorView.h>
#include <AppMan.h>
#include <imgui.h>

namespace fl
{
	namespace Editor
	{
		namespace View
		{

			constexpr float zoomSpeed = 1;
			bool panning = false;
			sf::Vector2i oldMousePos;

			void Update(sf::Int64 deltaTime)
			{
				if (panning)
				{
					auto view = AppMan::getWindow()->getView();
					view.move(sf::Vector2f(AppMan::getWindow()->mapPixelToCoords(oldMousePos) - AppMan::getWindow()->mapPixelToCoords(sf::Mouse::getPosition(*AppMan::getWindow()))));
					AppMan::getWindow()->setView(view);
					oldMousePos = sf::Mouse::getPosition(*AppMan::getWindow());
					if (!sf::Mouse::isButtonPressed(sf::Mouse::Middle)) panning = false;
				}
				else
				{
					if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
					{
						panning = true;
						oldMousePos = sf::Mouse::getPosition(*AppMan::getWindow());
					}
				}
			}
		}
	}
}
