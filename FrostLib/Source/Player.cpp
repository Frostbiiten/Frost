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

	float groundSpeed;
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

	//Floor detection raycasting variables
	float minADist = 1.f;
	float minAAngle = 0.f;
	float minBDist = 1.f;
	float minBAngle = 0.f;
	bool closerRay;
	bool equalRayDistance;
	float closerRayDistance = 0.f;
	float rayAngle = 0.f;
	b2Vec2 rayNormal;
	b2Vec2 rayHitPoint;

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

		//Defaults to main player controller if none is specified
		if (json["controller"] == 0)
			this->playerControls = &fl::ApplicationManager::mainPlayer;
		else
		{
			Debug::log("Alternative controllers not implemented, will default to main player");
			this->playerControls = &fl::ApplicationManager::mainPlayer;
		}
	}

	void Player::awake()
	{
		//Add rigidbody
		//Physics::rigidBody* rb = dynamic_cast<Physics::rigidBody*>(components[0].get());
	}
	
	void Player::update()
	{
		drawDebug();
		if (playerControls->button4 && currentState != PlayerState::Debug)
		{
			changeState(PlayerState::Debug);
		}

		Debug::drawLine(Physics::Box2dToPixelUnits(rayHitPoint), Physics::Box2dToPixelUnits(rayHitPoint) + Physics::Box2dToPixelUnits(rayNormal), sf::Color::White);

		ApplicationManager::imGuiText(std::to_string(rayAngle));
		ApplicationManager::imGuiText(std::to_string(groundSpeed));
		std::stringstream ss;
		ss << groundSpeed * cos(rayAngle);
		ss << ", ";
		ss << groundSpeed * sin(rayAngle);
		ApplicationManager::imGuiText(ss.str());
	}

	void Player::updateFloorRays()
	{
		//Rays extend twice the player's height
		std::vector<float>::iterator minA = std::min(rayAResults.fractions.begin(), rayAResults.fractions.end());
		std::vector<float>::iterator minB = std::min(rayBResults.fractions.begin(), rayBResults.fractions.end());

		//Predefine ray variables
		b2Vec2 rayANormal;
		b2Vec2 rayBNormal;
		float rayAAngle;
		float rayBAngle;
		b2Vec2 rayAHitPoint;
		b2Vec2 rayBHitPoint;

		//Get the variables for the closest rays
		if (minA != rayAResults.fractions.end())
		{
			minADist = *minA;
			int index = std::distance(rayAResults.fractions.begin(), minA);
			rayANormal = rayAResults.normals[index];
			minAAngle = Math::getAngle(rayAResults.normals[index]);
			rayAHitPoint = rayAResults.points[index];
		}
		if (minB != rayBResults.fractions.end())
		{
			minBDist = *minB;
			int index = std::distance(rayBResults.fractions.begin(), minB);
			rayBNormal = rayBResults.normals[index];
			minBAngle = Math::getAngle(rayBResults.normals[index]);
			rayBHitPoint = rayBResults.points[index];
		}

		//Right = true, Left = false
		//Defaults to the direction the player is moving in if the distances are too close (this is overriden later if they are not close)
		closerRay = playerVelocity.x > 0.f ? true : false;

		//Check if the distances are approximately equal
		equalRayDistance = Math::nearEqual(minADist, minBDist, 0.01f);

		//If they are not "equal", override the closest ray
		if (!equalRayDistance) closerRay = minBDist < minADist ? true : false;

		//Set variables based on closest ray
		if (closerRay)
		{
			rayHitPoint = rayBHitPoint;
			closerRayDistance = minBDist;
			rayAngle = round(minBAngle);
			rayNormal = rayBNormal;
		}
		else
		{
			rayHitPoint = rayAHitPoint;
			closerRayDistance = minADist;
			rayAngle = round(minAAngle);
			rayNormal = rayANormal;
		}
	}

	void Player::updatePosition()
	{
		transform.move(playerVelocity);
		transform.setRotation(Math::lerp(transform.getRotation(), 0.f, 0.2f));
		rb->getBody()->SetTransform(Physics::pixelToBox2dUnits(transform.getPosition()), Math::degToRad(transform.getRotation()));
	}

	void Player::groundTick()
	{
		updateRays();
		updateFloorRays();

		//Input
		float xInput = playerControls->directionalInput.x;
		bool xPositiveInput = xInput > 0.f;
		bool xPositiveSpeed = playerVelocity.x > 0.f;

		//If both the input direction and movement are in the same direction, use acceleration. If not, use deceleration
		if (playerControls->directionalInput.x != 0.f)
		{
			xPositiveInput == xPositiveSpeed ?
				groundSpeed -= playerControls->directionalInput.x * deceleration :
				groundSpeed -= playerControls->directionalInput.x * acceleration;
		}

		//The evaluated ground speed (split based on floor angle
		sf::Vector2f evaluatedGroundSpeed = Math::rotateVector(sf::Vector2f(groundSpeed, 0.f), rayAngle);
		playerVelocity.x = evaluatedGroundSpeed.x;
		playerVelocity.y = -evaluatedGroundSpeed.y;

		updatePosition();

		//If the closest ray is outside the player's bound (> half ray length), change to airborne
		if (closerRayDistance > 0.5f) changeState(PlayerState::Airborne);
	}

	void Player::airTick()
	{
		updateRays();
		updateFloorRays();

		//Input variables
		float xInput = playerControls->directionalInput.x;
		bool xPositiveInput = xInput > 0.f;
		bool xPositiveSpeed = playerVelocity.x > 0.f;

		//If both the input direction and movement are in the same direction, use acceleration. If not, use deceleration
		if (playerControls->directionalInput.x != 0.f)
		{
			xPositiveInput == xPositiveSpeed ?
				playerVelocity.x -= playerControls->directionalInput.x * deceleration :
				playerVelocity.x -= playerControls->directionalInput.x * acceleration;
		}

		//Apply air gravity
		playerVelocity.y += gravity;


		updatePosition();

		//If the closest ray is outside the player's bound (> half ray length), change to airborne
		if (closerRayDistance < 0.5f) changeState(PlayerState::Grounded);
	}

	float debugFlySpeed = 2.f;
	void Player::debugModeTick()
	{
		updateRays();
		updateFloorRays();
		if (playerControls->directionalInput.x != 0.f || playerControls->directionalInput.y != 0.f)
		{
			playerVelocity = -playerControls->directionalInput * debugFlySpeed;
			debugFlySpeed *= 1.005f;
		}
		else
		{
			debugFlySpeed = Math::lerp(debugFlySpeed, 2.f, 0.1f);
			playerVelocity = sf::Vector2f();
		}
		
		if (playerControls->button3)
		{
			changeState(PlayerState::Airborne);
		}

		updatePosition();
	}

	void Player::fixedUpdate()
	{
		//Update rays
		updateRays();

		switch (currentState)
		{
			case PlayerState::Grounded:
			{
				groundTick();
				break;
			}
			case PlayerState::Airborne:
			{
				airTick();
				break;
			}
			case PlayerState::Attacking:
			{

				break;
			}
			case PlayerState::Debug:
			{
				debugModeTick();
			}
		}
		
		//Update player camera position
		fl::Camera::updatePlayerCam(transform.getPosition() + playerRect / 2.f);
	}

	void Player::changeState(PlayerState newState)
	{
		//Custom switch logic for transitions, cancels, etc
		switch (newState)
		{
			case PlayerState::Grounded:
			{
				Debug::log("Transitioned to grounded");
				playerVelocity.y = 0.f;
				currentState = newState;
				/*Run groundTickt to prevent player from clipping
				through ground when falling at a high speed*/
				groundTick();
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
				Debug::log("Transitioned to attacking");
				currentState = newState;
				break;
			}
			case PlayerState::Debug:
			{
				Debug::log("Transitioned to debug mode");
				currentState = newState;
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
		Alternative rays for roof/wall detection

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

		//Serialize player
		if (playerControls == &fl::ApplicationManager::mainPlayer) json["controller"] = 0;
		else Debug::log("Alternate controllers have not been implemented yet. Player will not serialize fully");

		return json;
	}
}
