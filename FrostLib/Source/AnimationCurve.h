#pragma once
#include <Graph.h>
#include <MathUtil.h>
#include <json.hpp>

namespace fl
{
	class AnimationCurve : public Graph
	{
		Math::ClampMode clampMode;
		sf::Vector2f xRange;
	public:
		//Constructors
		AnimationCurve(Math::ClampMode clampMode = Math::ClampMode::Clamp);
		AnimationCurve(nlohmann::json json);

		//Get next node index from position
		int getIndex(sf::Vector2f positon);

		//Updates the range of the animationcurve
		void updateRange();
		float getRange();

		//Add/Move nodes
		void addNode(sf::Vector2f position);
		void addNode(nlohmann::json json);
		void addNode(sf::Vector2f position, sf::Vector2f handle);
		void addNode(sf::Vector2f position, sf::Vector2f handleL, sf::Vector2f handleR);
		void moveNode(sf::Vector2f newPosition, int index);
		void moveHandle(sf::Vector2f newPosition, int index, bool side);
		float evaluate(float x, float step = 0.01f);

		//Other
		nlohmann::json serialize();
	};
}

