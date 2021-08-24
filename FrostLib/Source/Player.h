#pragma once
#include <gameObject.h>
#include <Physics.h>
#include <box2d/box2d.h>

namespace fl
{
	//http://info.sonicretro.org/Sonic_Physics_Guide

	class Player : public gameObject
	{
		void updateRays();
		Physics::rayCallback raycast(b2RayCastInput& input, Layer layerMask);
		void fixedUpdate();
		void update();
	};
}

