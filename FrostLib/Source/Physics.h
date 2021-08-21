#pragma once
#include <gameObject.h>
#include <MathUtil.h>
#include <box2d/box2d.h> 

namespace fl
{
	namespace Physics
	{
		//World step
		void step();

		//Rigidbodies are by default are static and have rectangle colliders
		class rigidBody : public component
		{
			b2Body* body;
			b2Vec2 oldPos;
		public:
			rigidBody(b2BodyType bodyType = b2BodyType::b2_kinematicBody);

			void updateOldPos();

			//Updates old position for delta
			void preFixedUpdate();
			
			//Moves owner gameobject based on velocity
			void fixedUpdate();
		};
	}
}
