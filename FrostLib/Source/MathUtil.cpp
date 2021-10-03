#include <MathUtil.h>
#include <algorithm>
#include <cmath>
#include <box2d/box2d.h>
#include <iostream>


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

float fl::Math::roundToNearest(float n, float x)
{
	return round(n / x) * x;
}

sf::Vector2f fl::Math::lerpVec(sf::Vector2f a, sf::Vector2f b, float t)
{
	return sf::Vector2f( a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
}

float fl::Math::lerpAngle(float a, float b, float t)
{
    float delta = loop(0.f, (b - a), 360.f);
    if (delta > 180)
        delta -= 360;
    return a + delta * clamp(0.f, 1.f, t);
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

sf::Vector2f fl::Math::rotateVector(sf::Vector2f vector, float degrees)
{
	degrees = degToRad(degrees);
    return sf::Vector2f(
        vector.x * std::cosf(degrees) - vector.y * std::sin(degrees),
        vector.x * std::sin(degrees) + vector.y * std::cos(degrees)
    );
}

//Fix angles
float fl::Math::getAngle(sf::Vector2f vector)
{
	float value = (float)((atan2(vector.x, vector.y) / pi) * 180.f);
	if (value < 0) value += 360.f;
	return 360.f - value;
}

float fl::Math::getAngle(b2Vec2 vector)
{
	float value = (float)((atan2(vector.x, vector.y) / pi) * 180.f);
	if (value < 0) value += 360.f;
	return 360.f - value;
}

bool fl::Math::nearEqual(float a, float b, float epsilon)
{
    const float absA = fabs(a);
    const float absB = fabs(b);
    const float diff = fabs(a - b);

    if (a == b) return true;
    else if (a == 0 || b == 0 || diff < minNormal) return diff < (epsilon * minNormal);
    else return diff / (absA + absB) < epsilon;
}
