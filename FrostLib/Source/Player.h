#pragma once
#include <gameObject.h>
#include <Physics.h>
#include <box2d/box2d.h>

namespace fl
{
	//http://info.sonicretro.org/Sonic_Physics_Guide

	class Player : public gameObject
	{
		void awake();
		void update();
		void fixedUpdate();

		void updateRays();
		Physics::maskedRayCallback raycast(Physics::ray& input, Layer layerMask);

		void drawDebug();
	};
}

