#include <Physics.h>
#include <ApplicationManager.h>
#include <Utils.h>
#include <box2d/b2_body.h>

namespace fl
{
	namespace Physics
	{
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

		maskedRayCallback::maskedRayCallback(Layer layerMask)
		{
			this->layerMask = layerMask;
		}

		//Masked raycallback
		float maskedRayCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
		{
			auto comp = static_cast<component*>(fixture->GetBody()->GetUserData().data);
			if ((layerMask & comp->owner->layer) == comp->owner->layer)
			{
				bodies.push_back(fixture);
				points.push_back(point);
				normals.push_back(normal);
				fractions.push_back(fraction);
				components.push_back(comp);
			}

			return 1.f;
		}

		//Rigidbody
		rigidBody::rigidBody(gameObject* owner, b2Vec2 rectSize, b2BodyType bodyType, bool authoritativePosition)
		{
			this->owner = owner;
			enabled = true;
			name = "rigidbody";

			//Define body
			b2BodyDef bodyDef;
			bodyDef.type = bodyType;
			body = physicsWorld.CreateBody(&bodyDef);

			//Set "collider" using a polygon
			b2PolygonShape collider;
			collider.SetAsBox(rectSize.x, rectSize.y);

			//Finalize; user data is pointer to the component
			body->CreateFixture(&collider, 0.f);
			body->GetUserData().data = this;
			if (authoritativePosition)
			{
				owner->transform.setPosition(Box2dToPixelUnits(body->GetPosition()));
				owner->transform.setRotation(Math::radToDeg(body->GetAngle()));
			}
			else
			{
				auto pos = pixelToBox2dUnits(owner->transform.getPosition());
				auto rot = Math::degToRad(owner->transform.getRotation());
				body->SetTransform(pos, rot);
			}
		}

		rigidBody::~rigidBody()
		{
			if(body) physicsWorld.DestroyBody(body);
		}

		void rigidBody::destroyFixture(int index)
		{
			b2Fixture* x = body->GetFixtureList();
			int i = 0;
			while (i < index)
			{
				x = x->GetNext();
			}
			body->DestroyFixture(x);
		}

		void rigidBody::updateOldPos()
		{
			oldPos = body->GetPosition();
		}

		b2Body* rigidBody::getBody()
		{
			return body;
		}

		void rigidBody::preFixedUpdate()
		{
			updateOldPos();
		}

		void rigidBody::fixedUpdate()
		{
			if (body->GetType() != b2BodyType::b2_staticBody)
			{
				b2Vec2 deltaPos = body->GetPosition() - oldPos;
				owner->transform.move(Box2dToPixelUnits(deltaPos));
			}
			else
			{
				if (authoritativePosition)
					owner->transform.setPosition(Box2dToPixelUnits(body->GetPosition()));//This isn't deterministic
				else
					body->SetTransform(pixelToBox2dUnits(owner->transform.getPosition()), Math::degToRad(owner->transform.getRotation())); //This breaks some levels

			}
		}
	}
}
