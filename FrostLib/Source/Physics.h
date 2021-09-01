#pragma once
#include <gameObject.h>
#include <MathUtil.h>
#include <box2d/box2d.h>

namespace fl
{
	namespace Physics
	{
		//Main physics wolrd
		inline b2World physicsWorld(b2Vec2(0.f, -9.8f));

		//Conversion rate for Box2d to SFML units
		constexpr float pixelsPerUnit = 50;

		//Box2d does not operate with pixels as units
		inline b2Vec2 pixelToBox2dUnits(sf::Vector2f unit)
		{
			return b2Vec2(unit.x / pixelsPerUnit, unit.y / pixelsPerUnit);
		}

		inline sf::Vector2f Box2dToPixelUnits(b2Vec2 unit)
		{
			return sf::Vector2f(unit.x * pixelsPerUnit, unit.y * pixelsPerUnit);
		}

		inline sf::Vector2f b2dToVec(b2Vec2& vector)
		{
			return sf::Vector2f(vector.x, vector.y);
		}

		inline b2Vec2 vecToB2d(b2Vec2& vector)
		{
			return b2Vec2(vector.x, vector.y);
		}

		//World step
		void step();

		struct ray
		{
			b2Vec2 p1;
			b2Vec2 p2;
		};

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

		//Default raycallback
		struct maskedRayCallback : public b2RayCastCallback
		{
			Layer layerMask;
			std::vector<b2Fixture*> bodies;
			std::vector<b2Vec2> points;
			std::vector<b2Vec2> normals;
			std::vector<float> fractions;
			std::vector<component*> components;

			maskedRayCallback(Layer layerMask = Layer::All);

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

		public:
			rigidBody(gameObject* owner, b2Vec2 rectSize = b2Vec2(1.f, 1.f), b2BodyType bodyType = b2BodyType::b2_kinematicBody);
			~rigidBody();

			void destroyFixture(int index);

			void updateOldPos();

			b2Body* getBody();

			//Updates old position for delta
			void preFixedUpdate();
			
			//Moves owner gameobject based on velocity
			void fixedUpdate();
		};
	}
}
