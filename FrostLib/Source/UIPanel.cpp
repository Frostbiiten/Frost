#include "UIPanel.h"

namespace FrostLib
{
	void FrostLib::UIPanel::renderElement(sf::RenderWindow& output)
	{
		output.draw(shape);
	}
}
