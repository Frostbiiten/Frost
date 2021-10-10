#pragma once
#include <gameObject.h>
#include <Scene.h>
#include <Graph.h>

namespace fl
{
	struct SplineTerrain : public gameObject
	{
		Graph spline;
		std::vector<sf::Vector2f> outline;
		std::vector<sf::ConvexShape> shape;
		float simplifyThreshold;

		SplineTerrain(float simplifyThreshold = 1.f);
		SplineTerrain(nlohmann::json json);
		void generateOutline(float step = 10.f);
		void transformOutline();
		void simplifyOutline(float threshold = 1.f);
		void generateShape();
		void generateCollider();

		void setColor(sf::Color color = sf::Color::White);

		void update();

		nlohmann::json serialize();
	};
}

