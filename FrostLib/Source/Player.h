#pragma once
#include <gameObject.h>
#include <InputMan.h>
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
		Attacking,
		Debug
	};

	class Player : public gameObject
	{
	public:
		Player(nlohmann::json json, scene* scene);
		nlohmann::json serialize();
	protected:
		PlayerState currentState;
		fl::InputMan::inputMap* playerControls;

		void awake();
		void update();
		void fixedUpdate();

		void updateRays();
		Physics::maskedRayCallback raycast(Physics::ray& input, Layer layerMask);
		void updateFloorRays();
		void updateWallRays();

		void updatePosition();
		void updateRotation(bool truncate = false);
		void move(sf::Vector2f delta);

		bool testFloorCollision();

		void changeState(PlayerState newState);

		void groundTick();
		void airTick();
		void debugModeTick();

		void jump();

		void drawRay(Physics::ray& ray, sf::Color color);
		void drawDebug();
	};
}

