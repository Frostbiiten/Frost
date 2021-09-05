#pragma once
#include <SFML/System/Vector2.hpp>
namespace fl
{
	namespace Math
	{
		enum class ClampMode
		{
			Clamp,
			PingPong,
			Loop
		};

		//Linearly interpolates from a to b respectively
		float lerp(float a, float b, float t);

		//Inverts the lerp formula to return "t"
		float inverseLerp(float a, float b, float value);

		//Loops/Wraps a value between 0 and  
		float loop01(float t);

		//Loops/Wraps a value between a and b 
		float loop(float a, float b, float t);

		//Ping pong t between 0 and 1
		float pingPong01(float t);

		//Ping pong with a custom base and limit
		float pingPong(float a, float b, float t);

		//Clamps value between min and max using specified mode
		float clamp(float min, float max, float value, ClampMode mode = ClampMode::Clamp);

		//Converts radians to degrees
		float radToDeg(float radians);

		//Converts radians to degrees
		float degToRad(float degrees);

		//Vector math
		//Linearly interpolates from a to b respectively
		sf::Vector2f lerpVec(sf::Vector2f a, sf::Vector2f b, float t);

		// Returns the squared length of the vector
		float sqrMagnitude(sf::Vector2f vector);

		// Returns the length of the vector
		float magnitude(sf::Vector2f vector);

		// Returns Dot Product of two vectors.
		float dot(sf::Vector2f a, sf::Vector2f b);

		// Returns closest point on line segment
		sf::Vector2f closestPointOnSegment(sf::Vector2f a, sf::Vector2f b, sf::Vector2f point);
		
		//The the angle of p1 which is between p0 and p2
		float findAngle(sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2);
	};
}

