#pragma once
#include <SFML/Graphics/RoundedRectangleShape.hpp>
#include <UIElement.h>

namespace fl
{
	class UIPanel : public fl::UI::UIElement
	{
		sf::RoundedRectangleShape shape;

		void renderElement(sf::RenderWindow& output);
	};
}
