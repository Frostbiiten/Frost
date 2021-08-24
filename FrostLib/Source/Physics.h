#pragma once
#include <gameObject.h>
#include <MathUtil.h>
#include <box2d/box2d.h>

namespace fl
{
	namespace Physics
	{
		extern inline b2World physicsWorld;
		extern inline b2DynamicTree enemyTree;
		extern inline b2DynamicTree collectibleTree;
		extern inline b2DynamicTree landTree;
		extern inline b2DynamicTree defaultTree;

		//World step
		void step();

		inline b2Vec2 pixelToBox2dUnits(sf::Vector2f unit);
		inline sf::Vector2f Box2dToPixelUnits(b2Vec2 unit);

		//Default raycallback
		struct rayCallback : public b2RayCastCallback
		{
			std::vector<b2Fixture*> bodies;
			std::vector<b2Vec2> points;
			std::vector<b2Vec2> normals;
			std::vector<float> fractions;
			std::vector<component*> components;

			/// Called for each fixture found in the query. You control how the ray cast
			/// proceeds by returning a float:
			/// return -1: ignore this fixture and continue
			/// return 0: terminate the ray cast
			/// return fraction: clip the ray to this point
			/// return 1: don't clip the ray and continue
			/// @param fixture the fixture hit by the ray
			/// @param point the point of initial intersection
			/// @param normal the normal vector at the point of intersection
			/// @param fraction the fraction along the ray at the point of intersection
			/// @return -1 to filter, 0 to terminate, fraction to clip the ray for
			/// closest hit, 1 to continue
			float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction);
		};

		//Rigidbodies are by default are static and have rectangle colliders
		class rigidBody : public component
		{
			b2Body* body;
			b2Vec2 oldPos;
			b2AABB aabb;

			void updateAABB();

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
