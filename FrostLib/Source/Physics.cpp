#include <Physics.h>
#include <ApplicationManager.h>
#include <box2d/b2_body.h>

namespace fl
{
	namespace Physics
	{
		//Main physics wolrd
		inline b2World physicsWorld(b2Vec2(0.f, -9.8f));

		//Trees for querying
		inline b2DynamicTree enemyTree;
		inline b2DynamicTree collectibleTree;
		inline b2DynamicTree landTree;
		inline b2DynamicTree defaultTree; //This tree is the default tree for bodies. If they are not assigned to any other, they will be placed here.

		//Conversion rate for Box2d to SFML units
		constexpr float pixelsPerUnit = 100;

		//Box2d does not operate with pixels as units
		inline b2Vec2 pixelToBox2dUnits(sf::Vector2f unit)
		{
			return b2Vec2(unit.x / pixelsPerUnit, unit.y / pixelsPerUnit);
		}

		inline sf::Vector2f Box2dToPixelUnits(b2Vec2 unit)
		{
			return sf::Vector2f(unit.x * pixelsPerUnit, unit.y * pixelsPerUnit);
		}

		//Goes 1 step in the physics simulation
		void step()
		{
			physicsWorld.Step(1.f / fl::ApplicationManager::fixedTimestep, 1, 1);
		}

		//Custom raycallback
		float rayCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
		{
			bodies.push_back(fixture);
			points.push_back(point);
			normals.push_back(normal);
			fractions.push_back(fraction);
			components.push_back(static_cast<component*>(fixture->GetBody()->GetUserData().data));
			return 1.f;
		}

		//Rigidbody
		rigidBody::rigidBody(b2BodyType bodyType)
		{
			//Define body
			b2BodyDef bodyDef;
			bodyDef.type = bodyType;
			auto pos = pixelToBox2dUnits(owner->transform.getPosition());
			bodyDef.position.Set(pos.x, pos.y);

			//Set rotation (from degrees to radians)
			auto rot = Math::degToRad(owner->transform.getRotation());
			bodyDef.angle = rot;
			body = physicsWorld.CreateBody(&bodyDef);

			//Set "collider" using a polygon
			b2PolygonShape collider;
			auto scale = owner->transform.getScale();
			collider.SetAsBox(scale.x, scale.y);

			//Finalize; user data is pointer to owner
			body->CreateFixture(&collider, 0.f);
			body->GetUserData().data = owner;

			bool added = false;
			if ((owner->layer & Layer::Enemy) == Layer::Enemy)
			{
				enemyTree.CreateProxy(aabb, this);
				added = true;
			}
			if ((owner->layer & Layer::Land) == Layer::Land)
			{
				landTree.CreateProxy(aabb, this);
				added = true;
			}
			if ((owner->layer & Layer::Collectible) == Layer::Collectible)
			{
				collectibleTree.CreateProxy(aabb, this);
				added = true;
			}
			if (!added) defaultTree.CreateProxy(aabb, this);
		}

		void rigidBody::updateAABB()
		{
			body->GetFixtureList()->GetShape()->ComputeAABB(&aabb, body->GetTransform(), 0);
		}

		void rigidBody::updateOldPos()
		{
			oldPos = body->GetPosition();
		}

		void rigidBody::preFixedUpdate()
		{
			updateOldPos();
		}

		void rigidBody::fixedUpdate()
		{
			if (body->GetType() == b2BodyType::b2_kinematicBody)
			{
				b2Vec2 deltaPos = body->GetPosition() - oldPos;
				owner->transform.move(Box2dToPixelUnits(deltaPos));
			}
			else
			{
				owner->transform.move(Box2dToPixelUnits(body->GetPosition()));
			}

			updateAABB();
		}
	}
}
