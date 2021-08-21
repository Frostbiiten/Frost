#include <Physics.h>
#include <ApplicationManager.h>

namespace fl
{
	namespace Physics
	{
		inline b2World physicsWorld(b2Vec2(0.f, -9.8f));
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

		void step()
		{
			physicsWorld.Step(1.f / fl::ApplicationManager::fixedTimestep, 1, 1);	
		}

		rigidBody::rigidBody(b2BodyType bodyType)
		{
			b2BodyDef bodyDef;
			bodyDef.type = bodyType;
			auto pos = pixelToBox2dUnits(owner->transform.getPosition());
			bodyDef.position.Set(pos.x, pos.y);

			auto rot = Math::degToRad(owner->transform.getRotation());
			bodyDef.angle = rot;
			body = physicsWorld.CreateBody(&bodyDef);

			b2PolygonShape collider;
			auto scale = owner->transform.getScale();
			collider.SetAsBox(scale.x, scale.y);

			body->CreateFixture(&collider, 0.f);
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
			b2Vec2 deltaPos = body->GetPosition() - oldPos;
			owner->transform.move(sf::Vector2f(deltaPos.x, deltaPos.y));
		}
	}
}
