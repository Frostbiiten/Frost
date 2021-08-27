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
