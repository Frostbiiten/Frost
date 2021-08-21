#pragma once
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
	};
}

