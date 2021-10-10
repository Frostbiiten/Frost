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
	float jmp = 6.5f;
	bool jumping = false;
	bool jumpEscapingGround = false;
	bool fixedUpdateJump = false;
	float gravity = 0.21875f;
	float friction = 0.046875f;
	float slopeFactor = 0.125f;
	float acceleration = 0.046875f;
	float deceleration = 0.5f;
	float topSpeed = 16.f;
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
	MovementDirection movementDirection;

	//Floor detection raycasting variables
	float minADist = 1.f;
	float minAAngle = 0.f;
	float minBDist = 1.f;
	float minBAngle = 0.f;

	float minEDist = 1.f;
	float minEAngle = 0.f;
	float minFDist = 1.f;
	float minFAngle = 0.f;

	float minCDist = 1.f;
	float minCAngle = 0.f;
	float minDDist = 1.f;
	float minDAngle = 0.f;

	bool closerRay = true;
	bool equalRayDistance = false;
	float closerRayDistance = 0.f;
	float closerRoofRayDistance = 0.f;
	float rayAngle = 0.f; //ray angle in counter-clockwise degrees
	float roofRayAngle = 0.f; //ray angle in counter-clockwise degrees
	b2Vec2 rayNormal{0, 1};
	b2Vec2 rayHitPoint{};

	Player::Player(nlohmann::json json)
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

	void Player::awake()
	{
		//Add rigidbody
		//Physics::rigidBody* rb = dynamic_cast<Physics::rigidBody*>(components[0].get());
	}
	void Player::update()
	{
		drawDebug();
		if (playerControls->button4Down)
		{
			if (currentState == PlayerState::Debug) changeState(PlayerState::Airborne);
			else changeState(PlayerState::Debug);
		}

		std::stringstream position;
		position << "position: " << transform.getPosition().x << ", " << transform.getPosition().y;

		switch (currentState)
		{
			case PlayerState::Grounded:
				if (playerControls->button1Down)
				{
					fixedUpdateJump = true;
				}
				ApplicationManager::imGuiText("State: Grounded");
				break;
			case PlayerState::Airborne:
				ApplicationManager::imGuiText("State: Airborne");
				break;
			case PlayerState::Debug:
				if (playerControls->button3Down) Debug::dumpPhysicsWorld();
				ApplicationManager::imGuiText("State: Debug");
				break;
			case PlayerState::Attacking:
				ApplicationManager::imGuiText("State: Attacking");
				break;
		}
		ApplicationManager::imGuiText(position.str());
		ApplicationManager::imGuiText(std::to_string(closerRoofRayDistance));
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

	//Updates the position of rays
	void Player::updateRays()
	{
		sf::Vector2f position = transform.getPosition();
		sf::Vector2f centerPosition = transform.getPosition() + playerRect / 2.f;

		//todo: overhaul ground rays to start from middle of playerRect y

		if (currentState == PlayerState::Grounded)
		{
			//360 ray system
			rayA.p1 = Physics::pixelToBox2dUnits(centerPosition + Math::rotateVector(sf::Vector2f(playerRect.x / -2.f, 0.f), transform.getRotation()));
			rayA.p2 = Physics::pixelToBox2dUnits(centerPosition + Math::rotateVector(sf::Vector2f(playerRect.x / -2.f, playerRect.y), transform.getRotation()));

			rayB.p1 = Physics::pixelToBox2dUnits(centerPosition + Math::rotateVector(sf::Vector2f(playerRect.x / 2.f, 0.f), transform.getRotation()));
			rayB.p2 = Physics::pixelToBox2dUnits(centerPosition + Math::rotateVector(sf::Vector2f(playerRect.x / 2.f, playerRect.y), transform.getRotation()));
		}
		else
		{
			//Air physics makes no distinction of angles
			rayA.p1 = Physics::pixelToBox2dUnits(sf::Vector2f(position.x, centerPosition.y));
			rayA.p2 = Physics::pixelToBox2dUnits(position + sf::Vector2f(0.f, playerRect.y * 1.5f));

			rayB.p1 = Physics::pixelToBox2dUnits(sf::Vector2f(position.x + playerRect.x, centerPosition.y));
			rayB.p2 = Physics::pixelToBox2dUnits(position + sf::Vector2f(playerRect.x, playerRect.y * 1.5f));
		}

		rayAResults = raycast(rayA, Layer::All);
		rayBResults = raycast(rayB, Layer::All);

		//Rays for wall detection
		//Wall detection is run before movement is applied, so it MUST add the player's velocity to account for the lag -> 0.001 offset so it goes further out than floor and ceiling rays
		if (currentState == PlayerState::Grounded)
		{
			if (std::abs(std::floor(playerVelocity.y)) != 0.f)
			{
				rayE.p1 = Physics::pixelToBox2dUnits(centerPosition);
				rayE.p2 = Physics::pixelToBox2dUnits(centerPosition + Math::rotateVector(sf::Vector2f(playerRect.x / 2.f + 0.001f, 0.f), transform.getRotation()));

				rayF.p1 = Physics::pixelToBox2dUnits(centerPosition);
				rayF.p2 = Physics::pixelToBox2dUnits(centerPosition + Math::rotateVector(sf::Vector2f(-playerRect.x / 2.f - 0.001f, 0.f), transform.getRotation()));
			}
			else
			{
				const sf::Vector2f offset(0.f, 10.f);

				rayE.p1 = Physics::pixelToBox2dUnits(centerPosition + offset);
				rayE.p2 = Physics::pixelToBox2dUnits(centerPosition + offset + Math::rotateVector(sf::Vector2f(playerRect.x / 2.f + 0.001f, 0.f), transform.getRotation()));

				rayF.p1 = Physics::pixelToBox2dUnits(centerPosition + offset);
				rayF.p2 = Physics::pixelToBox2dUnits(centerPosition + offset + Math::rotateVector(sf::Vector2f(-playerRect.x / 2.f - 0.001f, 0.f), transform.getRotation()));
			}
		}
		else
		{
			//Don't rotate rays while airborne
			rayE.p1 = Physics::pixelToBox2dUnits(centerPosition);
			rayE.p2 = Physics::pixelToBox2dUnits(centerPosition + sf::Vector2f(playerRect.x / 2.f + 0.001f, 0.f));

			rayF.p1 = Physics::pixelToBox2dUnits(centerPosition);
			rayF.p2 = Physics::pixelToBox2dUnits(centerPosition + sf::Vector2f(-playerRect.x / 2.f - 0.001f, 0.f));
		}

		rayEResults = raycast(rayE, Layer::All);
		rayFResults = raycast(rayF, Layer::All);

		//Rays for roof detection

		if (currentState == PlayerState::Grounded)
		{
			rayC.p1 = Physics::pixelToBox2dUnits(centerPosition + Math::rotateVector(sf::Vector2f(playerRect.x / -2.f, 0.f), transform.getRotation()));
			rayC.p2 = Physics::pixelToBox2dUnits(centerPosition + Math::rotateVector(sf::Vector2f(playerRect.x / -2.f, playerRect.y / -2.f), transform.getRotation()));

			rayD.p1 = Physics::pixelToBox2dUnits(centerPosition + Math::rotateVector(sf::Vector2f(playerRect.x / 2.f, 0.f), transform.getRotation()));
			rayD.p2 = Physics::pixelToBox2dUnits(centerPosition + Math::rotateVector(sf::Vector2f(playerRect.x / 2.f, playerRect.y / -2.f), transform.getRotation()));
		}
		else
		{
			//Air physics makes no distinction of angles
			rayC.p1 = Physics::pixelToBox2dUnits(sf::Vector2f(position.x, centerPosition.y));
			rayC.p2 = Physics::pixelToBox2dUnits(position);

			rayD.p1 = Physics::pixelToBox2dUnits(sf::Vector2f(position.x + playerRect.x, centerPosition.y));
			rayD.p2 = Physics::pixelToBox2dUnits(sf::Vector2f(position.x + playerRect.x, position.y));
		}

		rayCResults = raycast(rayC, Layer::All ^ Layer::JumpThroughLand);
		rayDResults = raycast(rayD, Layer::All ^ Layer::JumpThroughLand);
	}

	//Casts rays
	Physics::maskedRayCallback Player::raycast(Physics::ray& input, Layer layerMask)
	{
		Physics::maskedRayCallback ray = Physics::maskedRayCallback{ layerMask };
		Physics::physicsWorld.RayCast(&ray, input.p1, input.p2);
		return ray;
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
			minADist = 2.f;
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
			minBDist = 2.f;
		}

		//Right = true, Left = false
		//Defaults to the direction the player is moving in if the distances are too close (this is overriden later if they are not close)
		if (currentState == PlayerState::Grounded) closerRay = groundSpeed > 0.f ? true : false;
		else closerRay = playerVelocity.x > 0.f ? true : false;

		//Check if the distances are approximately equal
		equalRayDistance = Math::nearEqual(minADist, minBDist, 0.01f);

		//If they are not "equal", override the closest ray
		if (!equalRayDistance) closerRay = minBDist < minADist ? true : false;

		closerRayDistance = 2.f;
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
	void Player::updateRoofRays()
	{
		std::vector<float>::iterator minC = std::min_element(rayCResults.fractions.begin(), rayCResults.fractions.end());
		std::vector<float>::iterator minD = std::min_element(rayDResults.fractions.begin(), rayDResults.fractions.end());

		//Predefine ray variables
		b2Vec2 rayCNormal;
		b2Vec2 rayDNormal;
		b2Vec2 rayCHitPoint;
		b2Vec2 rayDHitPoint;

		//Get the variables for the closest rays
		if (minC != rayCResults.fractions.end())
		{
			minCDist = *minC;
			int index = std::distance(rayCResults.fractions.begin(), minC);
			rayCNormal = rayCResults.normals[index];
			rayCHitPoint = rayCResults.points[index];
		}
		else
		{
			minCDist = 2.f;
		}
		if (minD != rayDResults.fractions.end())
		{
			minDDist = *minD;
			int index = std::distance(rayDResults.fractions.begin(), minD);
			rayDNormal = rayDResults.normals[index];
			rayDHitPoint = rayDResults.points[index];
		}
		else
		{
			minDDist = 2.f;
		}

		bool closerRoofRay = playerVelocity.x > 0.f ? true : false;

		//Check if the distances are approximately equal
		bool equalRoofRayDistance = Math::nearEqual(minCDist, minDDist, 0.01f);

		//If they are not "equal", override the closest ray
		if (!equalRoofRayDistance) closerRoofRay = minDDist < minCDist ? true : false;

		closerRoofRayDistance = 2.f;
		//Set variables based on closest ray
		if (closerRoofRay)
		{
			closerRoofRayDistance = minDDist;
			roofRayAngle = minDAngle;
		}
		else
		{
			closerRoofRayDistance = minCDist;
			roofRayAngle = minCAngle;
		}
	}

	//Applies velocity to transforms
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
	void Player::updateRotation(bool truncate)
	{
		if (truncate) transform.setRotation(Math::roundToNearest(360.f - angle, 90.f));
		else transform.setRotation(angle);
	}
	void Player::move(sf::Vector2f delta)
	{
		transform.move(delta);
		rb->getBody()->SetTransform(Physics::pixelToBox2dUnits(transform.getPosition()), Math::degToRad(transform.getRotation()));
	}
	void Player::updateMovementDirection()
	{
		if (std::abs(playerVelocity.x) >= std::abs(playerVelocity.y))
		{
			if (playerVelocity.x > 0.f) movementDirection = MovementDirection::Right;
			else movementDirection = MovementDirection::Left;
		}
		else
		{
			if (playerVelocity.y > 0.f) movementDirection = MovementDirection::Down;
			else movementDirection = MovementDirection::Up;
		}
	}

	//Test for floor collision
	bool Player::testFloorCollision()
	{
		float floorDistance = (closerRayDistance * playerRect.y) - playerRect.y / 2.f;
		if (currentState == PlayerState::Grounded)
		{
			return (floorDistance < std::min(std::abs(playerVelocity.x) + 4.f, 14.f)); //Add line for playerVelocity.y when on walls
		}
		else if (currentState == PlayerState::Airborne)
		{
			updateMovementDirection();
			if(closerRayDistance <= 0.55f)
			{
				if (movementDirection == MovementDirection::Down)
				{
					if (floorDistance >= -(playerVelocity.y + 8)) return true;
				}
				else if (movementDirection == MovementDirection::Right || movementDirection == MovementDirection::Left)
				{
					if (playerVelocity.y >= 0.f) return true;
				}
			}
			return false;
		}

		fl::Debug::log("no floor collision state override! Defaulting to no collision");
		return false;
	}

	//Change state
	void Player::changeState(PlayerState newState)
	{
		//Custom switch logic for transitions, cancels, etc
		switch (newState)
		{
			case PlayerState::Grounded:
			{
				if (currentState == PlayerState::Airborne)
				{
					jumping = false;

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

						relativePlayerVelocity.y = 0.f;
						playerVelocity = Math::rotateVector(relativePlayerVelocity, 360.f - rayAngle);
					}
					transform.setRotation(360.f - rayAngle);
				}
				currentState = newState;
				break;
			}
			case PlayerState::Airborne:
			{
				currentState = newState;
				break;
			}
			case PlayerState::Attacking:
			{
				currentState = newState;
				break;
			}
			case PlayerState::Debug:
			{
				Debug::log("Entering Debug Mode");
				currentState = newState;
			}
		}
	}

	//State ticks
	void Player::groundTick()
	{
		//Slope factor
		groundSpeed -= slopeFactor * std::sin(Math::degToRad(rayAngle));

		//Input
		float xInput = playerControls->directionalInput.x;
		bool xPositiveInput = xInput > 0.f;
		bool xPositiveSpeed = groundSpeed > 0.f;

		//If both the input direction and movement are in the same direction, use acceleration. If not, use deceleration
		if (playerControls->directionalInput.x != 0.f && std::abs(groundSpeed) < topSpeed)
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

		//TODO: RUN ROOF COLLISION TEST
		updateWallRays();

		//Walls
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

		//Jumping
		if (fixedUpdateJump)
		{
			jump();
			fixedUpdateJump = false;
			jumpEscapingGround = true;
			jumping = true;
			return;
		}

		//if (jumping) return;

		//The evaluated ground speed (split based on floor angle
		sf::Vector2f evaluatedGroundSpeed = Math::rotateVector(sf::Vector2f(groundSpeed, 0.f), rayAngle);
		playerVelocity.x = evaluatedGroundSpeed.x;
		playerVelocity.y = -evaluatedGroundSpeed.y;

		//Truncate angle to nearest 90 degrees to stop twitching
		angle = Math::roundToNearest(360.f - rayAngle, 90.f);

		//Update position based on velocity
		updatePosition();

		//Update rays after updating position
		updateRays();
		updateFloorRays();

		//Test if still on floor
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
			sf::Vector2f alignDelta(0.0f, (closerRayDistance * playerRect.y) - playerRect.y / 2.f);
			alignDelta = Math::rotateVector(alignDelta, transform.getRotation());
			move(alignDelta);
			updateRotation();
			updateRays();
			updateFloorRays();
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
		updateRoofRays();
		updateWallRays();
		
		//Air has no distinction between acceleration and deceleration
		playerVelocity.x += playerControls->directionalInput.x * acceleration * 2.f;

		//Variable jump height
		if (!playerControls->button1 && jumping)
		{
			if (playerVelocity.y < -4.f)
				playerVelocity.y = -4.f;
		}

		//wall detect (ran before roof to stop roof-clipping
		if (minEDist <= 1.f && playerVelocity.x > 0.f)
		{
			float intersectionDistance = (1.f - minEDist) * playerRect.x / 2.f;
			move(sf::Vector2f(-intersectionDistance + 0.0001f, 0.f)); //Add small offset so ray is still in wall (no twitching)
			playerVelocity.x = 0.f;
		}
		if (minFDist <= 1.f && playerVelocity.x < 0.f)
		{
			float intersectionDistance = (1.f - minFDist) * playerRect.x / 2.f;
			move(sf::Vector2f(intersectionDistance - 0.0001f, 0.f)); //Add small offset so ray is still in wall (no twitching)
			playerVelocity.x = 0.f;
		}

		//Update rays after moving
		updateRays();
		updateFloorRays();
		updateRoofRays();

		//roof detect
		if (closerRoofRayDistance <= 1.f)
		{
			//1. remove velocity perpendicular to wall
			sf::Vector2f localRoofVelocity = Math::rotateVector(playerVelocity, roofRayAngle);
			localRoofVelocity.y = 0.f;
			playerVelocity = Math::rotateVector(localRoofVelocity, 360.f - roofRayAngle);

			//2. push out of wall
			//std::cout << 1.f - closerRoofRayDistance << '\n';
			float distance = (1.f - closerRoofRayDistance) * (playerRect.y / 2.f);
			move(sf::Vector2f(0.f, distance + 0.0001f));
		}

		//Update rays after moving
		updateRays();
		updateFloorRays();
		updateRoofRays();

		//Apply air gravity
		playerVelocity.y += gravity;
		playerVelocity.y = std::clamp(playerVelocity.y, -ySpeedLimit, ySpeedLimit);
		
		//Apply air drag
		if (playerVelocity.y < 0 && playerVelocity.y > -4)
		{
			playerVelocity.x -= ((playerVelocity.x / 0.125f) / 256.f);
		}

		//Rotate towards 0 degrees
		if (transform.getRotation() > 180.f)
			angle = Math::moveTowards(transform.getRotation(), 360.f, 2.8125f);
		else
			angle = Math::moveTowards(transform.getRotation(), 0.f, 2.8125f);

		//Update transform
		updatePosition();
		updateRotation();

		//If floor is detected
		if (testFloorCollision() && !jumpEscapingGround)//REPLACE WITH ESCAPING FLOOR FOR JUMP
		{
			changeState(PlayerState::Grounded);
		}
		else
		{
			//Check if you are no longer escaping the ground with jump (replace soon)
			if (closerRayDistance > 0.6f)
			{
				jumpEscapingGround = false;
			}
		}
	}
	float debugFlySpeed = 2.f;
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

		updatePosition();
	}

	//Jump
	void Player::jump()
	{
		changeState(PlayerState::Airborne);
		playerVelocity += sf::Vector2f(rayNormal.x, -rayNormal.y) * jmp;
	}

	//Debug
	void Player::drawRay(Physics::ray& ray, sf::Color color)
	{
		Debug::drawLine(Physics::Box2dToPixelUnits(ray.p1), Physics::Box2dToPixelUnits(ray.p2), color);
		Debug::drawRectangle(Physics::Box2dToPixelUnits(ray.p1), sf::Vector2f(0.005f, 0.005f), 0.f, 1.f, sf::Color(255, 255, 255, color.a));
	}
	void Player::drawDebug()
	{
		//Draw player hitbox
		sf::RectangleShape rect;
		rect.setSize(playerRect);
		rect.setOrigin(rect.getSize() / 2.f);
		rect.setPosition(transform.getPosition() + rect.getSize() / 2.f);
		if (currentState == PlayerState::Grounded)
			rect.setRotation(360.f - rayAngle);
		else
			rect.setRotation(transform.getRotation());

		rect.setFillColor(sf::Color::Transparent);
		rect.setOutlineColor(sf::Color::Blue);
		rect.setOutlineThickness(1.f);
		ApplicationManager::getWindow()->draw(rect);

		sf::Int8 rayTransparency = 50;
		//Draw player rays
		drawRay(rayA, sf::Color(0, 255, 0, rayTransparency));
		drawRay(rayB, sf::Color(0, 255, 255, rayTransparency));

		drawRay(rayE, sf::Color(255, 192, 203, rayTransparency));
		drawRay(rayF, sf::Color(255, 0, 0, rayTransparency));

		drawRay(rayC, sf::Color(0, 0, 255, rayTransparency));
		drawRay(rayD, sf::Color(255, 255, 0, equalRayDistance));

		//Draw hit ray
		Debug::drawLine(Physics::Box2dToPixelUnits(rayHitPoint), Physics::Box2dToPixelUnits(rayHitPoint) + Physics::Box2dToPixelUnits(rayNormal), sf::Color(255, 255, 255, rayTransparency));
	}
}
