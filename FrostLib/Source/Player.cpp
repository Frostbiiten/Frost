#include <Player.h>
#include <Debug.h>
#include <gameObject.h>
#include <Physics.h>
#include <Utils.h>
#include <Camera.h>

namespace fl
{
	Physics::ray rayA;
	Physics::ray rayB;
	Physics::ray rayC;
	Physics::ray rayD;
	Physics::ray rayE;
	Physics::ray rayF;

	Physics::maskedRayCallback rayAResults;
	Physics::maskedRayCallback rayBResults;
	Physics::maskedRayCallback rayCResults;
	Physics::maskedRayCallback rayDResults;
	Physics::maskedRayCallback rayEResults;
	Physics::maskedRayCallback rayFResults;

	//Default player hitbox/bounds
	sf::Vector2f playerRect{ 19, 39 };

	Player::Player(nlohmann::json json, scene* scene)
	{
		name = json["name"];
		uuid = uuids::uuid::from_string(json["uuid"].get<std::string>());
		layer = json["layer"];
		active = json["active"];
		transform = deserializeTransform(json["transform"]);	
		components.push_back(std::make_unique<Physics::rigidBody>(this, Physics::pixelToBox2dUnits(playerRect)));
	}

	void Player::awake()
	{
		//Add rigidbody
		//Physics::rigidBody* rb = dynamic_cast<Physics::rigidBody*>(components[0].get());
	}
	
	void Player::update()
	{
		Physics::rigidBody* derivedPointer = dynamic_cast<Physics::rigidBody*>(components[0].get());
		drawDebug();
	}

	void Player::fixedUpdate()
	{
		//Update rays
		updateRays();

		switch (currentState)
		{
		case PlayerState::Grounded:

			break;
		case PlayerState::Airborne:
			break;
		case PlayerState::Attacking:
			break;
		}
		
		//Update player camera position
		fl::Camera::updatePlayerCam(transform.getPosition() + playerRect / 2.f);
	}

	void Player::updateRays()
	{
		sf::Vector2f position = transform.getPosition();

		rayA.p1 = Physics::pixelToBox2dUnits(position + sf::Vector2f(0.f, playerRect.y));
		rayA.p2 = Physics::pixelToBox2dUnits(position + sf::Vector2f(0.f, playerRect.y / 2.f));
		rayAResults = raycast(rayA, Layer::All);

		rayB.p1 = Physics::pixelToBox2dUnits(position + playerRect);
		rayB.p2 = Physics::pixelToBox2dUnits(position + sf::Vector2f(playerRect.x, playerRect.y / 2.f));
		rayBResults = raycast(rayB, Layer::All);

		rayC.p1 = Physics::pixelToBox2dUnits(position);
		rayC.p2 = Physics::pixelToBox2dUnits(position + sf::Vector2f(0.f , playerRect.y / 2.f));
		rayCResults = raycast(rayC, Layer::All);

		rayD.p1 = Physics::pixelToBox2dUnits(position + sf::Vector2f(playerRect.x, 0.f));
		rayD.p2 = Physics::pixelToBox2dUnits(position + sf::Vector2f(playerRect.x, playerRect.y / 2.f));
		rayDResults = raycast(rayD, Layer::All);

		rayE.p1 = Physics::pixelToBox2dUnits(position + sf::Vector2f(0.f, playerRect.y / 2.f));
		rayE.p2 = Physics::pixelToBox2dUnits(position + sf::Vector2f(playerRect.x, playerRect.y) / 2.f);
		rayEResults = raycast(rayE, Layer::All);

		rayF.p1 = Physics::pixelToBox2dUnits(position + sf::Vector2f(playerRect.x, playerRect.y / 2.f));
		rayF.p2 = Physics::pixelToBox2dUnits(position + sf::Vector2f(playerRect.x, playerRect.y) / 2.f);
		rayFResults = raycast(rayF, Layer::All);
	}

	Physics::maskedRayCallback Player::raycast(Physics::ray& input, Layer layerMask)
	{
		Physics::maskedRayCallback ray = Physics::maskedRayCallback{layerMask};
		Physics::physicsWorld.RayCast(&ray, input.p1, input.p2);
		return ray;
	}

	void Player::drawRay(Physics::ray& ray, sf::Color color)
	{
		Debug::drawLine(Physics::Box2dToPixelUnits(ray.p1), Physics::Box2dToPixelUnits(ray.p2), color);
		Debug::drawRectangle(Physics::Box2dToPixelUnits(ray.p1), sf::Vector2f(0.01f, 0.01f), 0.f, 1.f, sf::Color::White);
	}

	void Player::drawDebug()
	{
		//Draw player hitbox
		//Debug::drawRectangle(transform.getPosition(), playerRect, transform.getRotation(), 0.5f, sf::Color::Blue);

		//Draw player rays
		drawRay(rayA, sf::Color::Green);
		drawRay(rayB, sf::Color(0, 255, 255));
		drawRay(rayC, sf::Color::Blue);
		drawRay(rayD, sf::Color::Yellow);
		drawRay(rayE, sf::Color(255, 16, 240));
		drawRay(rayF, sf::Color::Red);
	}

	nlohmann::json Player::serialize()
	{
		//Start with serializing the most important parts of a gameObject
		nlohmann::json json = serializeBasic();
		json["transform"] = serializeTransform();
		json["type"] = "player";

		for (auto& child : children)
		{
			json["children"].push_back(child->serialize());
		}

		return json;
	}
}
