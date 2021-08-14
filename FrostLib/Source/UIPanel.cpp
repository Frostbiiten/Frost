#include "UIPanel.h"

namespace fl
{
	void fl::UIPanel::renderElement(sf::RenderWindow& output)
	{
		output.draw(shape);
	}
}
