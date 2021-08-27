#include "Player.h"
#include <Debug.h>
#include <Physics.h>
#include <Utils.h>

namespace fl
{
	//rayMid -> EF, rayLeft = AB, rayRight -> CD
	Physics::ray rayMid;
	Physics::ray rayLeft;
	Physics::ray rayRight;

	Physics::maskedRayCallback rayMidResults;
	Physics::maskedRayCallback rayLeftResults;
	Physics::maskedRayCallback rayRightResults;
	Physics::ray testRay;

	//Default player hitbox/bounds
	sf::Vector2f playerRect{ 19, 39 };
	
	void Player::awake()
	{
		//Add rigidbody
		components.push_back(std::make_unique<Physics::rigidBody>(this, Physics::pixelToBox2dUnits(playerRect)));
		//Physics::rigidBody* rb = dynamic_cast<Physics::rigidBody*>(components[0].get());
		std::cout << static_cast<int>(layer) << std::endl;
	}
	
	void Player::update()
	{
		Physics::rigidBody* derivedPointer = dynamic_cast<Physics::rigidBody*>(components[0].get());
		drawDebug();
	}

	void Player::fixedUpdate()
	{
		updateRays();
	}

	void Player::updateRays()
	{
		sf::Vector2f position = transform.getPosition();

		//EF
		rayMid.p1 = Physics::pixelToBox2dUnits(position + sf::Vector2f(0.1f, playerRect.y / 2));
		rayMid.p2 = Physics::pixelToBox2dUnits(position + sf::Vector2f(playerRect.x, playerRect.y / 2));
		rayMidResults = raycast(rayMid, Layer::All);

		//AB
		rayLeft.p1 = Physics::pixelToBox2dUnits(position);
		rayLeft.p2 = Physics::pixelToBox2dUnits(position + sf::Vector2f(0.f, playerRect.y));
		rayLeftResults = raycast(rayLeft, Layer::All);

		//CD
		rayRight.p1 = Physics::pixelToBox2dUnits(position + sf::Vector2f(playerRect.x, 0.f));
		rayRight.p2 = Physics::pixelToBox2dUnits(position + playerRect);
		rayRightResults = raycast(rayRight, Layer::All);

		//Testray
		testRay.p1 = Physics::pixelToBox2dUnits(position - sf::Vector2f(100.f, 100.f));
		testRay.p2 = Physics::pixelToBox2dUnits(position + playerRect + sf::Vector2f(100.f, 100.f));
		auto results = raycast(testRay, Layer::Player);
		std::cout << " ";
	}

	Physics::maskedRayCallback Player::raycast(Physics::ray& input, Layer layerMask)
	{
		Physics::maskedRayCallback ray = Physics::maskedRayCallback{layerMask};
		Physics::physicsWorld.RayCast(&ray, input.p1, input.p2);
		return ray;
	}

	void Player::drawDebug()
	{
		//Draw player hitbox
		Debug::drawRectangle(transform.getPosition(), playerRect, transform.getRotation(), 0.5f, sf::Color::Blue);

		Debug::drawLine(Physics::Box2dToPixelUnits(testRay.p1), Physics::Box2dToPixelUnits(testRay.p2), sf::Color::Green);

		//Draw player rays
		Debug::drawLine(Physics::Box2dToPixelUnits(rayMid.p1), Physics::Box2dToPixelUnits(rayMid.p2));
		Debug::drawLine(Physics::Box2dToPixelUnits(rayLeft.p1), Physics::Box2dToPixelUnits(rayLeft.p2));
		Debug::drawLine(Physics::Box2dToPixelUnits(rayRight.p1), Physics::Box2dToPixelUnits(rayRight.p2));
		//Debug::drawLineThick(Physics::Box2dToPixelUnits(rayUL.p1), Physics::Box2dToPixelUnits(rayUL.p2), 1.f);
		//Debug::drawLineThick(Physics::Box2dToPixelUnits(rayDR.p1), Physics::Box2dToPixelUnits(rayDR.p2), 1.f);
		//Debug::drawLineThick(Physics::Box2dToPixelUnits(rayDL.p1), Physics::Box2dToPixelUnits(rayDL.p2), 1.f);
	}
}
