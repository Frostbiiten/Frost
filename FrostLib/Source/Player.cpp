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

	float groundSpeed = 0.f;
	float gravity = 0.21875f;
	float friction = 0.046875f;
	float slopeFactor = 0.125f;
	float acceleration = 0.046875f;
	float deceleration = 0.5f;
	float topSpeed = 6.f;
	float angle = 0.f; //Player angle in clockwise degrees
	float rotationMultiple = 45.f;
	float ySpeedLimit = 16.f;
	sf::Vector2f maxVelocity = { 32.f, 32.f };

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
	sf::Vector2f playerVelocity{ 0.f, 0.f };

	//Floor detection raycasting variables
	float minADist = 1.f;
	float minAAngle = 0.f;
	float minBDist = 1.f;
	float minBAngle = 0.f;

	float minEDist = 1.f;
	float minEAngle = 0.f;
	float minFDist = 1.f;
	float minFAngle = 0.f;

	bool closerRay = true;
	bool equalRayDistance = false;
	float closerRayDistance = 0.f;
	float rayAngle = 0.f; //ray angle in counter-clockwise degrees
	b2Vec2 rayNormal{0, 1};
	b2Vec2 rayHitPoint{};

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

		//ApplicationManager::imGuiText("Rayangle: " + std::to_string(rayAngle));
		//ApplicationManager::imGuiText("Angle   : " + std::to_string(transform.getRotation()));
		std::stringstream velo;
		velo << "velocity: " << playerVelocity.x << ", " << playerVelocity.y;
		ApplicationManager::imGuiText(velo.str());

		std::stringstream position;
		position << "position: " << transform.getPosition().x << ", " << transform.getPosition().y;
		ApplicationManager::imGuiText(position.str());
	}

	void Player::updateFloorRays()
	{
		//Rays extend twice the player's height
		std::vector<float>::iterator minA = std::min_element(rayAResults.fractions.begin(), rayAResults.fractions.end());
		std::vector<float>::iterator minB = std::min_element(rayBResults.fractions.begin(), rayBResults.fractions.end());

		//Predefine ray variables
		b2Vec2 rayANormal;
		b2Vec2 rayBNormal;
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
		else
		{
			minADist = 1.f;
		}
		if (minB != rayBResults.fractions.end())
		{
			minBDist = *minB;
			int index = std::distance(rayBResults.fractions.begin(), minB);
			rayBNormal = rayBResults.normals[index];
			minBAngle = Math::getAngle(rayBResults.normals[index]);
			rayBHitPoint = rayBResults.points[index];
		}
		else
		{
			minBDist = 1.f;
		}

		//Right = true, Left = false
		//Defaults to the direction the player is moving in if the distances are too close (this is overriden later if they are not close)
		if (currentState == PlayerState::Grounded)
		{
			closerRay = groundSpeed > 0.f ? true : false;
		}
		else
		{
			closerRay = playerVelocity.x > 0.f ? true : false;
		}

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

	void Player::updateWallRays()
	{
		//Rays extend twice the player's height
		std::vector<float>::iterator minE = std::min_element(rayEResults.fractions.begin(), rayEResults.fractions.end());
		std::vector<float>::iterator minF = std::min_element(rayFResults.fractions.begin(), rayFResults.fractions.end());

		//Predefine ray variables
		b2Vec2 rayENormal;
		b2Vec2 rayFNormal;
		b2Vec2 rayEHitPoint;
		b2Vec2 rayFHitPoint;

		//Get the variables for the closest rays
		if (minE != rayEResults.fractions.end())
		{
			minEDist = *minE;
			int index = std::distance(rayEResults.fractions.begin(), minE);
			rayENormal = rayEResults.normals[index];
			rayEHitPoint = rayEResults.points[index];
		}
		else
		{
			minEDist = 2.f;
		}
		if (minF != rayFResults.fractions.end())
		{
			minFDist = *minF;
			int index = std::distance(rayFResults.fractions.begin(), minF);
			rayFNormal = rayFResults.normals[index];
			rayFHitPoint = rayFResults.points[index];
		}
		else
		{
			minFDist = 2.f;
		}
	}

	void Player::updatePosition()
	{
		sf::Vector2f clampedVelocity{
			std::clamp(playerVelocity.x, -maxVelocity.x, maxVelocity.y),
			std::clamp(playerVelocity.y, -maxVelocity.y, maxVelocity.y)
		};
		transform.move(clampedVelocity);
		//transform.setPosition(transform.getPosition() + sf::Vector2f(std::floor(playerVelocity.x), std::floor(playerVelocity.y)));
		rb->getBody()->SetTransform(Physics::pixelToBox2dUnits(transform.getPosition()), Math::degToRad(transform.getRotation()));
	}

	void Player::move(sf::Vector2f delta)
	{
		transform.move(delta);
		rb->getBody()->SetTransform(Physics::pixelToBox2dUnits(transform.getPosition()), Math::degToRad(transform.getRotation()));
	}

	void Player::updateRotation(bool truncate)
	{
		if (truncate)
		{
			transform.setRotation(Math::roundToNearest(360.f - angle, 90.f));
		}
		else
		{
			transform.setRotation(angle);
		}
	}

	bool Player::testFloorCollision()
	{
		float floorDistance = -(playerRect.y / 4.f) + closerRayDistance * playerRect.y;
		return std::clamp(closerRayDistance, 0.25f, 0.65f) == closerRayDistance;
		//return (floorDistance < std::min(std::abs(playerVelocity.x) + 4.f, 14.f) && floorDistance > 0.25f);
	}

	void Player::groundTick()
	{
		//Slope factor
		groundSpeed -= slopeFactor * std::sin(Math::degToRad(rayAngle));

		//Input
		float xInput = playerControls->directionalInput.x;
		bool xPositiveInput = xInput > 0.f;
		bool xPositiveSpeed = groundSpeed > 0.f;

		//If both the input direction and movement are in the same direction, use acceleration. If not, use deceleration
		if (playerControls->directionalInput.x != 0.f && groundSpeed < topSpeed)
		{
			xPositiveInput == xPositiveSpeed ?
				groundSpeed += playerControls->directionalInput.x * acceleration :
				groundSpeed += playerControls->directionalInput.x * deceleration;
		}

		//Friction
		if (playerControls->directionalInput.x == 0.0f)
		{
			bool speedDirection = groundSpeed > 0.0f;
			float absGroundSpeed = std::abs(groundSpeed);
			groundSpeed = absGroundSpeed - friction;
			if (groundSpeed < 0.f)
			{
				groundSpeed = 0.0f;
			}
			else if (!speedDirection)
			{
				groundSpeed = -groundSpeed;
			}
		}

		//TODO: RUN WALL/ROOF COLLISION TEST
		updateWallRays();
		if (minEDist <= 1.f && groundSpeed > 0.f)
		{
			float intersectionDistance = (1.f - minEDist) * playerRect.x / 2.f;
			move(sf::Vector2f(-intersectionDistance + 0.0001f, 0.f)); //Add small offset so ray is still in wall (no twitching)
			groundSpeed = 0.f;
		}
		if (minFDist <= 1.f && groundSpeed < 0.f)
		{
			float intersectionDistance = (1.f - minFDist) * playerRect.x / 2.f;
			move(sf::Vector2f(intersectionDistance - 0.0001f, 0.f)); //Add small offset so ray is still in wall (no twitching)
			groundSpeed = 0.f;
		}

		//The evaluated ground speed (split based on floor angle
		sf::Vector2f evaluatedGroundSpeed = Math::rotateVector(sf::Vector2f(groundSpeed, 0.f), rayAngle);
		playerVelocity.x = evaluatedGroundSpeed.x;
		playerVelocity.y = -evaluatedGroundSpeed.y;

		//Truncate angle to nearest 90 degrees to stop twitching
		angle = Math::roundToNearest(360.f - rayAngle, 90.f);

		//Update position based on velocity
		updatePosition();

		//Rays
		updateRays();
		updateFloorRays();

		//If the distance of the ray is in the allowable range, stay on floor. Else, go airborne
		//TODO: variable max ray length depending on speed
		if (testFloorCollision())
		{
			//Check if player should slip
			if (std::abs(groundSpeed) < 2.5f && (rayAngle == std::clamp(rayAngle, 35.f, 326.f)))
			{
				//Lock for -> 16 ms per frame for 30 frames
				playerControls->lockDirectionalInput(ApplicationManager::fixedTimestep * 30);

				// Check if player should fall?
				if (rayAngle == std::clamp(rayAngle, 69.f, 293.f))
				{
					//Detach from ground
					changeState(PlayerState::Airborne);
				}
				else
				{
					//Depending on what side the slope is, add or subtract 0.5 from Ground Speed
					if (rayAngle < 180) groundSpeed -= 0.5f;
					else groundSpeed += 0.5f;
				}
			}

			//Move sonic (update ground angle + position) to adhere to ground
			sf::Vector2f alignDelta(0.0f, -(playerRect.y - (closerRayDistance * playerRect.y * 2.f)));
			alignDelta = Math::rotateVector(alignDelta, transform.getRotation());
			move(alignDelta);
			updateRotation();
		}
		else
		{
			changeState(PlayerState::Airborne);
		}
	}

	void Player::airTick()
	{
		// Note: Regardless of the Player's Ground Angle, their airborne sensors do not rotate. Air collision essentially ignores the Player's angle and treats it as floor mode at all times. 
		updateRays();
		updateFloorRays();

		//Air has no distinction between acceleration and deceleration
		playerVelocity.x += playerControls->directionalInput.x * acceleration * 2.f;

		//Apply air gravity
		playerVelocity.y += gravity;
		playerVelocity.y = std::clamp(playerVelocity.y, -ySpeedLimit, ySpeedLimit);
		
		//Apply air drag
		if (playerVelocity.y < 0 && playerVelocity.y > -4)
		{
			playerVelocity.y -= ((playerVelocity.x / 0.125f) / 256.f);
		}

		//Rotate towards 0 degrees
		if (transform.getRotation() > 180.f)
			angle = Math::moveTowards(transform.getRotation(), 360.f, 2.8125f);
		else
			angle = Math::moveTowards(transform.getRotation(), 0.f, 2.8125f);

		//Update transform
		updatePosition();
		updateRotation();

		//Tick lock timer
		if (playerControls->directionalLocked)
		{
			playerControls->directionalTimerMs -= ApplicationManager::fixedTimestep;
			if (playerControls->directionalTimerMs <= 0)
				playerControls->directionalLocked = false;
		}

		//Check for ground transition
		//TODO: variable max ray length depending on speed
		if (testFloorCollision())
		{
			//Find how much speed should be retained

			if (rayAngle < 23.f && rayAngle > 339.f)
			{
				groundSpeed = playerVelocity.x;
			}
			else
			{
				sf::Vector2f normalizedVelocity = playerVelocity / Math::magnitude(playerVelocity);
				sf::Vector2f normalizedNormal = Physics::Box2dToPixelUnits(rayNormal); //Normal must be "normalized" because it is being converted from box2d units
				normalizedNormal = normalizedNormal / Math::magnitude(normalizedNormal);
				float dotProduct = Math::dot(normalizedVelocity, normalizedNormal);

				//Check player velocity relative to the floor
				//There are still some inaccuracies?
				sf::Vector2f relativePlayerVelocity = Math::rotateVector(playerVelocity, rayAngle);

				if (relativePlayerVelocity.x > 0.f)
					groundSpeed = Math::magnitude(playerVelocity) * (1.f - std::abs(dotProduct));
				else
					groundSpeed = -Math::magnitude(playerVelocity) * (1.f - std::abs(dotProduct));
			}

			changeState(PlayerState::Grounded);
		}
	}

	float debugFlySpeed = 2.f;
	bool button2Old = false;
	bool button2Ref = false;
	void Player::debugModeTick()
	{
		updateRays();
		updateFloorRays();
		if (playerControls->directionalInput.x != 0.f || playerControls->directionalInput.y != 0.f)
		{
			playerVelocity = sf::Vector2f(playerControls->directionalInput.x, -playerControls->directionalInput.y) * debugFlySpeed;
			debugFlySpeed *= 1.005f;
		}
		else
		{
			debugFlySpeed = Math::lerp(debugFlySpeed, 2.f, 0.1f);
			playerVelocity = sf::Vector2f();
		}

		if (playerControls->button1)
		{
			transform.rotate(-0.5f);
		}
		if (playerControls->button2)
		{
			transform.rotate(0.5f);
		}

		if (playerControls->button3)
		{
			changeState(PlayerState::Airborne);
		}

		button2Old = button2Ref;
		button2Ref = playerControls->button2;
		if(button2Old != button2Ref && button2Ref)
		{
			Debug::log("");
			Debug::log("Physics world debugger");
			Debug::log("-----------------------");

			Debug::log("\tPhysics world body list");
			Debug::log("\t-----------------------");
			auto element = Physics::physicsWorld.GetBodyList();
			for (int i = 0; i < Physics::physicsWorld.GetBodyCount(); ++i)
			{
				Physics::rigidBody* comp = static_cast<Physics::rigidBody*>(element->GetUserData().data);
				Debug::log("\t\t" + std::to_string(i) + ": " + comp->owner->name);
				element = element->GetNext();
				sf::Vector2f position = comp->owner->transform.getPosition();
				sf::Vector2f scale = comp->owner->transform.getScale();
				Debug::log("\t\t Position: " + std::to_string(position.x) + ", " + std::to_string(position.y));
				Debug::log("\t\t Rotation: " + std::to_string(comp->owner->transform.getRotation()));
				Debug::log("\t\t Scale: " + std::to_string(scale.x) + ", " + std::to_string(scale.y));
			}
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
		sf::Vector2f centerPosition = transform.getPosition() + playerRect / 2.f;

		if (currentState == PlayerState::Grounded)
		{
			//360 ray system
			rayA.p1 = Physics::pixelToBox2dUnits(centerPosition + Math::rotateVector(sf::Vector2f(playerRect.x / -2.f, playerRect.y / -2.f), transform.getRotation()));
			rayA.p2 = Physics::pixelToBox2dUnits(centerPosition + Math::rotateVector(sf::Vector2f(playerRect.x / -2.f, playerRect.y * 1.5f), transform.getRotation()));

			rayB.p1 = Physics::pixelToBox2dUnits(centerPosition + Math::rotateVector(sf::Vector2f(playerRect.x / 2.f, playerRect.y / -2.f), transform.getRotation()));
			rayB.p2 = Physics::pixelToBox2dUnits(centerPosition + Math::rotateVector(sf::Vector2f(playerRect.x / 2.f, playerRect.y * 1.5f), transform.getRotation()));
		}
		else
		{
			//Air physics makes no distinction of angles
			rayA.p1 = Physics::pixelToBox2dUnits(position + sf::Vector2f(0.f, 0.f));
			rayA.p2 = Physics::pixelToBox2dUnits(position + sf::Vector2f(0.f, playerRect.y * 2.f));

			rayB.p1 = Physics::pixelToBox2dUnits(position + sf::Vector2f(playerRect.x, 0.f));
			rayB.p2 = Physics::pixelToBox2dUnits(position + sf::Vector2f(playerRect.x, playerRect.y * 2.f));
		}

		rayAResults = raycast(rayA, Layer::All);
		rayBResults = raycast(rayB, Layer::All);

		/*
		Alternative rays for roof/wall detection

		rayC.p1 = Physics::pixelToBox2dUnits(position);
		rayC.p2 = Physics::pixelToBox2dUnits(position + sf::Vector2f(0.f , playerRect.y / 2.f));
		rayCResults = raycast(rayC, Layer::All);

		rayD.p1 = Physics::pixelToBox2dUnits(position + sf::Vector2f(playerRect.x, 0.f));
		rayD.p2 = Physics::pixelToBox2dUnits(position + sf::Vector2f(playerRect.x, playerRect.y / 2.f));
		rayDResults = raycast(rayD, Layer::All);
		*/

		//Rays for wall detection
		//Wall detection is run before movement is applied, so it MUST add the player's velocity to account for the lag
		if (currentState == PlayerState::Grounded)
		{
			if (std::abs(std::floor(groundSpeed)) > 0.f)
			{
				rayE.p1 = Physics::pixelToBox2dUnits(centerPosition);
				rayE.p2 = Physics::pixelToBox2dUnits(centerPosition + Math::rotateVector(sf::Vector2f(playerRect.x / 2.f, 0.f), transform.getRotation()));

				rayF.p1 = Physics::pixelToBox2dUnits(centerPosition);
				rayF.p2 = Physics::pixelToBox2dUnits(centerPosition + Math::rotateVector(sf::Vector2f(-playerRect.x / 2.f, 0.f), transform.getRotation()));
			}
			else
			{
				//If player is not moving, offset rays by 8 down in y axis
				const sf::Vector2f offset(0.f, 10.f);

				rayE.p1 = Physics::pixelToBox2dUnits(centerPosition + offset);
				rayE.p2 = Physics::pixelToBox2dUnits(centerPosition + offset + Math::rotateVector(sf::Vector2f(playerRect.x / 2.f, 0.f), transform.getRotation()));

				rayF.p1 = Physics::pixelToBox2dUnits(centerPosition + offset);
				rayF.p2 = Physics::pixelToBox2dUnits(centerPosition + offset + Math::rotateVector(sf::Vector2f(-playerRect.x / 2.f, 0.f), transform.getRotation()));
			}


		}
		else
		{
			//Don't rotate rays while airborne
			rayE.p1 = Physics::pixelToBox2dUnits(centerPosition);
			rayE.p2 = Physics::pixelToBox2dUnits(centerPosition + sf::Vector2f(playerRect.x / 2.f, 0.f));

			rayF.p1 = Physics::pixelToBox2dUnits(centerPosition);
			rayF.p2 = Physics::pixelToBox2dUnits(centerPosition + sf::Vector2f(-playerRect.x / 2.f, 0.f));
		}

		rayEResults = raycast(rayE, Layer::All);
		rayFResults = raycast(rayF, Layer::All);
	}

	Physics::maskedRayCallback Player::raycast(Physics::ray& input, Layer layerMask)
	{
		Physics::maskedRayCallback ray = Physics::maskedRayCallback{ layerMask };
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
		sf::RectangleShape rect;
		rect.setSize(playerRect);
		rect.setOrigin(rect.getSize() / 2.f);
		rect.setPosition(transform.getPosition() + rect.getSize() / 2.f); //Position is broken?
		if (currentState == PlayerState::Grounded)
			rect.setRotation(360.f - rayAngle);
		else
			rect.setRotation(transform.getRotation());

		rect.setFillColor(sf::Color::Transparent);
		rect.setOutlineColor(sf::Color::Blue);
		rect.setOutlineThickness(1.f);
		ApplicationManager::getWindow()->draw(rect);

		//Draw player rays
		/*
		drawRay(rayA, sf::Color::Green);
		drawRay(rayB, sf::Color(0, 255, 255));

		drawRay(rayE, sf::Color(255, 192, 203));
		drawRay(rayF, sf::Color::Red);
		*/

		//Draw hit ray
		//Debug::drawLine(Physics::Box2dToPixelUnits(rayHitPoint), Physics::Box2dToPixelUnits(rayHitPoint) + Physics::Box2dToPixelUnits(rayNormal), sf::Color::White);
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
