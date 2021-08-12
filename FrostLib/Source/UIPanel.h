#pragma once
#include <SFML/Graphics/RoundedRectangleShape.hpp>
#include "UIElement.h"

namespace FrostLib
{
	class UIPanel : public FrostLib::UI::UIElement
	{
		sf::RoundedRectangleShape shape;

		void renderElement(sf::RenderWindow& output);
	};
}
