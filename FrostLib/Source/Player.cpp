#include <Player.h>
#include <ApplicationManager.h>
#include <Camera.h>
#include <Debug.h>
#include <gameObject.h>
#include <Physics.h>
#include <Utils.h>

namespace fl
{
	/*
	//General Object Variables/Attributes
	X Position : The X - coordinate of the object's centre.
	Y Position : The Y - coordinate of the object's centre.
	X Speed : The speed at which the object is moving horizontally.
	Y Speed : The speed at which the object is moving vertically.
	Ground Speed : The speed at which the object is moving on the ground.
	Ground Angle : The object's angle on the ground.
	Width Radius : The object's width from the origin pixel left and right.
	Height Radius : The object's height from the origin pixel up and down.

	//Sonic's Variables
	Push Radius : Sonic's width from the origin pixel left and right (for pushing).
	Slope Factor : The current slope factor value being used.

	//Sonic's Speed constants
	acc : 0.046875; acceleration
	dec : 0.5; deceleration
	frc : 0.046875; friction(same as acc)
	top: 6; top horizontal speed
	slp : 0.125; slope factor when walking / running
	slprollup : 0.078125; slope factor when rolling uphill
	slprolldown : 0.3125; slope factor when rolling downhill
	fall : 2.5; tolerance ground speed for sticking to wallsand ceilings

	//Sonic's Airborne Speed Constants
	air : 0.09375; air acceleration(2x acc)
	jmp: 6.5; jump force(6 for knuckles)
	grv: 0.21875; gravity
	*/

	float gravity = 0.021875f;
	float slopeFactor = 0.125f;
	float acceleration = 0.046875f;
	float deceleration = 0.5f;

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

	Physics::rigidBody* rb;
	//Default player hitbox/bounds
	sf::Vector2f playerRect{ 19, 39 };
	sf::Vector2f playerVelocity;

	Player::Player(nlohmann::json json, scene* scene)
	{
		name = json["name"];
		uuid = uuids::uuid::from_string(json["uuid"].get<std::string>());
		layer = json["layer"];
		active = json["active"];
		transform = deserializeTransform(json["transform"]);	
		components.push_back(std::make_unique<Physics::rigidBody>(this, Physics::pixelToBox2dUnits(playerRect)));
		rb = dynamic_cast<Physics::rigidBody*>(components[0].get());
		currentState = PlayerState::Airborne;
	}

	void Player::awake()
	{
		//Add rigidbody
		//Physics::rigidBody* rb = dynamic_cast<Physics::rigidBody*>(components[0].get());
	}
	
	void Player::update()
	{
		drawDebug();
	}

	void groundTick()
	{
		//Rays extend twice the player's height
		std::vector<float>::iterator minA = std::min(rayAResults.fractions.begin(), rayAResults.fractions.end());
		std::vector<float>::iterator minB = std::min(rayBResults.fractions.begin(), rayBResults.fractions.end());
		float minADist = 1.f;
		float minAAngle = 0.f;
		float minBDist = 1.f;
		float minBAngle = 0.f;

		if (minA != rayAResults.fractions.end())
		{
			minADist = *minA;
			int index = std::distance(rayAResults.fractions.begin(), minA);
			minAAngle = Math::getAngle(rayAResults.normals[index]) - 90.f;
		}
		if (minB != rayBResults.fractions.end())
		{
			minBDist = *minB;
			int index = std::distance(rayBResults.fractions.begin(), minB);
			minBAngle = Math::getAngle(rayBResults.normals[index]) - 90.f;
		}

		//Right = true, Left = false
		bool closerRay = playerVelocity.x > 0.f ? true : false;
		bool equalRayDistance = Math::nearEqual(minADist, minBDist, 0.5f);
		if (!equalRayDistance) closerRay = minBDist < minBDist ? true : false;
		float closerRayDistance = closerRay ? minBDist : minADist;
		float rayAngle = round(closerRay ? minBAngle : minAAngle);

		//Movement

		if ((playerVelocity.x > 0.f) != (ApplicationManager::mainPlayer.directionalInput.x > 0.f))
		{
			playerVelocity.x -= ApplicationManager::mainPlayer.directionalInput.x * acceleration;
		}
		else
		{
			playerVelocity.x -= ApplicationManager::mainPlayer.directionalInput.x * deceleration;
		}

		//Slope physics
		//playerVelocity.x -= slopeFactor * cosf(rayAngle);
		//playerVelocity.y += slopeFactor * sinf(rayAngle);

		playerVelocity.y += ((closerRayDistance - 0.5f) * playerRect.y) / 2;
		transform.move(playerVelocity);
		transform.setRotation(Math::lerp(transform.getRotation(), 0.f, 0.2f));
		rb->getBody()->SetTransform(Physics::pixelToBox2dUnits(transform.getPosition()), Math::degToRad(transform.getRotation()));

		//If the closest ray is outside the player's bound (> half ray length), change to airborne
		if (closerRayDistance > 0.6f)
		{
			changeState(PlayerState::Airborne);
		}
	}

	void Player::fixedUpdate()
	{
		//Update rays
		updateRays();

		switch (currentState)
		{
			case PlayerState::Grounded:
			{


				break;
			}
			case PlayerState::Airborne:
			{

				break;
			}
			case PlayerState::Attacking:
			{

				break;
			}
		}
		
		//Update player camera position
		fl::Camera::updatePlayerCam(transform.getPosition() + playerRect / 2.f);
	}

	void Player::changeState(PlayerState newState)
	{
		//Custom switch logic
		switch (newState)
		{
			case PlayerState::Grounded:
			{
				Debug::log("Transitioned to grounded");
				playerVelocity.y = 0.f;
				currentState = newState;
				break;
			}
			case PlayerState::Airborne:
			{
				Debug::log("Transitioned to airborne");
				currentState = newState;
				break;
			}
			case PlayerState::Attacking:
			{
				currentState = newState;
				break;
			}
		}
	}

	void Player::updateRays()
	{
		sf::Vector2f position = transform.getPosition();

		rayA.p1 = Physics::pixelToBox2dUnits(position + sf::Vector2f(0.f, 0.f));
		rayA.p2 = Physics::pixelToBox2dUnits(position + sf::Vector2f(0.f, playerRect.y * 2.f));
		rayAResults = raycast(rayA, Layer::All);

		rayB.p1 = Physics::pixelToBox2dUnits(position + sf::Vector2f(playerRect.x, 0.f));
		rayB.p2 = Physics::pixelToBox2dUnits(position + sf::Vector2f(playerRect.x, playerRect.y * 2.f));
		rayBResults = raycast(rayB, Layer::All);

		/*
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
		*/
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
		/*
		drawRay(rayC, sf::Color::Blue);
		drawRay(rayD, sf::Color::Yellow);
		drawRay(rayE, sf::Color(255, 16, 240));
		drawRay(rayF, sf::Color::Red);
		*/
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
