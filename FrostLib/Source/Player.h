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
	protected:
		PlayerState currentState;
		fl::InputMan::inputMap* playerControls;

		void awake();
		void update();
		void updateFloorRays();

		//Moves playbody in physics world and render world according to player velocity
		void updatePosition();
		void move(sf::Vector2f delta);
		void updateRotation();

		//The player's default ground state code, runs on fixedUpdate
		void groundTick();
		void airTick();
		void debugModeTick();
		void fixedUpdate();

		void changeState(PlayerState newState);

		void updateRays();
		Physics::maskedRayCallback raycast(Physics::ray& input, Layer layerMask);

		void drawRay(Physics::ray& ray, sf::Color color);

		void drawDebug();
		nlohmann::json serialize();
	};
}

