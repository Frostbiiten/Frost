#include <MathUtil.h>
#include <algorithm>
#include <cmath>

constexpr double pi = 3.1415926535897932384626433832795028841971693993751058209749445923078164062;
constexpr float pi_f = 3.1415926535897932384626433832795028841971693993751058209749445923078164062;

float fl::Math::lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

float fl::Math::inverseLerp(float a, float b, float value)
{
    if (a != b)
        return (value - a) / (b - a);
    else
        return 0.0f;
}

float fl::Math::loop01(float t)
{
    return clamp(t - floorf(t), 0.f, 1.f);
}

float fl::Math::loop(float a, float b, float t)
{
	return fl::Math::lerp(a, b, loop01(t));
}

float fl::Math::pingPong01(float t)
{
	t = loop(0.f, 2.f, t);
	return 1.f - fabsf(t - 1.f);
}

float fl::Math::pingPong(float a, float b, float t)
{
	return fl::Math::lerp(a, b, pingPong01(t));
}

float fl::Math::clamp(float min, float max, float value, ClampMode mode)
{
	switch (mode)
	{
	case fl::Math::ClampMode::Clamp:
		return(std::min(std::max(value, min), max));
		break;
	case fl::Math::ClampMode::PingPong:
		return pingPong(min, max, value);
		break;
	case fl::Math::ClampMode::Loop:
		return loop(min, max, value);
		break;
	}
}

float fl::Math::radToDeg(float radians)
{
	return (radians * 180.f) / pi_f;
}

float fl::Math::degToRad(float degrees)
{
	return (degrees * pi_f) / 180.f;
}

sf::Vector2f fl::Math::lerpVec(sf::Vector2f a, sf::Vector2f b, float t)
{
	return sf::Vector2f( a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
}

// Returns the length of this vector (RO).
float fl::Math::sqrMagnitude(sf::Vector2f vector)
{
    return vector.x * vector.x + vector.y * vector.y;
}

float fl::Math::magnitude(sf::Vector2f vector)
{
    return sqrtf(vector.x * vector.x + vector.y * vector.y);
}

float fl::Math::dot(sf::Vector2f a, sf::Vector2f b)
{
    return a.x * b.x + a.y * b.y; 
}

//https://stackoverflow.com/a/9557244 (not my code)
sf::Vector2f fl::Math::closestPointOnSegment(sf::Vector2f a, sf::Vector2f b, sf::Vector2f point)
{
    sf::Vector2f AP = point - a;
    sf::Vector2f AB = b - a;

    float magnitudeAB = sqrMagnitude(AB);
    float ABAPproduct = dot(AP, AB);
    float distance = ABAPproduct / magnitudeAB;

    if (distance < 0) return a;
    else if (distance > 1) return b;
    else return a + AB * distance;
}

float fl::Math::findAngle(sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2)
{
	float b = pow(p1.x - p0.x, 2) + pow(p1.y - p0.y, 2);
	float a = pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2);
	float c = pow(p2.x - p0.x, 2) + pow(p2.y - p0.y, 2);
	return radToDeg(acos((a + b - c) / sqrtf(4 * a * b)));
}
