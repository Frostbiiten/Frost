#pragma once
#include <gameObject.h>
#include <Physics.h>
#include <SFML/Graphics/Color.hpp>
#include <box2d/box2d.h>

namespace fl
{
	//http://info.sonicretro.org/Sonic_Physics_Guide

	enum class PlayerState
	{
		Grounded,
		Airborne,
		Attacking
	};

	class Player : public gameObject
	{
		PlayerState currentState;

		void awake();
		void update();
		void fixedUpdate();

		void updateRays();
		Physics::maskedRayCallback raycast(Physics::ray& input, Layer layerMask);

		void drawRay(Physics::ray& ray, sf::Color color);

		void drawDebug();
	};
}

