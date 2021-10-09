#pragma once
#include <SFML/System/Vector2.hpp>
#include <box2d/box2d.h>

namespace fl
{
	namespace Math
	{
		constexpr inline double pi = 3.1415926535897932384626433832795028841971693993751058209749445923078164062f;
		constexpr inline float pi_f = 3.1415926535897932384626433832795028841971693993751058209749445923078164062f;
		constexpr inline double minNormal = 1.17549435E-38;

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

		float roundToNearest(float n, float x);

		float sign(float f);

		float moveTowards(float current, float target, float maxDelta);

		//Vector math
		//Linearly interpolates from a to b respectively
		sf::Vector2f lerpVec(sf::Vector2f a, sf::Vector2f b, float t);

		//Correctly lerps an angle by the shortest path
		float lerpAngle(float a, float b, float t);

		// Returns the squared length of the vector
		float sqrMagnitude(sf::Vector2f vector);

		// Returns the length of the vector
		float magnitude(sf::Vector2f vector);

		// Returns Dot Product of two vectors.
		float dot(sf::Vector2f a, sf::Vector2f b);

		// Returns closest point on line segment
		sf::Vector2f closestPointOnSegment(sf::Vector2f a, sf::Vector2f b, sf::Vector2f point);

		sf::Vector2f rotateVector(sf::Vector2f vector, float degrees);

		//Angle between a vector and the y axis
		float getAngle(sf::Vector2f vector);
		float getAngle(b2Vec2 vector);

		//Find if floating point numbers are near equal
		bool nearEqual(float a, float b, float epsilon);
	};
}

