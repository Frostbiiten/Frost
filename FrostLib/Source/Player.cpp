#include "Player.h"
#include <Debug.h>
#include <Physics.h>
#include <Utils.h>

namespace fl
{
	//Respectively right, left, up-right, up-left, down-right, and down-left
	b2RayCastInput rayR;
	b2RayCastInput rayL;
	b2RayCastInput rayUR;
	b2RayCastInput rayUL;
	b2RayCastInput rayDR;
	b2RayCastInput rayDL;

	b2RayCastOutput rayROut;
	b2RayCastOutput rayLOut;
	b2RayCastOutput rayUROut;
	b2RayCastOutput rayULOut;
	b2RayCastOutput rayDROut;

	sf::Vector2f playerRect{ 19, 39 };

	void Player::updateRays()
	{
		sf::Vector2f pos = transform.getPosition();
		sf::Vector2f rightRayPos = sf::Vector2f(pos.x + playerRect.x / 2, pos.y);
		sf::Vector2f leftRayPos = sf::Vector2f(pos.x - playerRect.x / 2, pos.y);

		//Mid rays
		rayR.p1 = fl::Physics::pixelToBox2dUnits(pos);
		rayR.p2 = fl::Physics::pixelToBox2dUnits(rightRayPos);
		rayL.p1 = fl::Physics::pixelToBox2dUnits(pos);
		rayL.p2 = fl::Physics::pixelToBox2dUnits(leftRayPos);

		//Up rays
		rayUR.p1 = fl::Physics::pixelToBox2dUnits(rightRayPos);
		rayUR.p2 = fl::Physics::pixelToBox2dUnits(sf::Vector2f(rightRayPos.x, pos.y + playerRect.y / 2));
		rayUL.p1 = fl::Physics::pixelToBox2dUnits(leftRayPos);
		rayUL.p2 = fl::Physics::pixelToBox2dUnits(sf::Vector2f(leftRayPos.x, pos.y + playerRect.y / 2));

		//Down rays
		rayDR.p1 = fl::Physics::pixelToBox2dUnits(rightRayPos);
		rayDR.p2 = fl::Physics::pixelToBox2dUnits(sf::Vector2f(rightRayPos.x, pos.y - playerRect.y / 2));
		rayDL.p1 = fl::Physics::pixelToBox2dUnits(leftRayPos);
		rayDL.p2 = fl::Physics::pixelToBox2dUnits(sf::Vector2f(leftRayPos.x, pos.y - playerRect.y / 2));

		//raycast(rayR, player);
	}

	Physics::rayCallback Player::raycast(b2RayCastInput& input, Layer layerMask)
	{
		return Physics::rayCallback();
	}

	/*
	Physics::rayCallback Player::raycast(b2RayCastInput& input, Layer layerMask)
	{
		Physics::rayCallback finalRay;
		if ((layerMask & Layer::Enemy) == Layer::Enemy)
			Physics::enemyTree.RayCast(&finalRay, input);
		if ((layerMask & Layer::Land) == Layer::Land)
			Physics::landTree.RayCast(&finalRay, input);
		if ((layerMask & Layer::Collectible) == Layer::Collectible)
			Physics::collectibleTree.RayCast(&finalRay, input);
		return finalRay;
	}
	*/

	void Player::update()
	{
		Debug::drawRectangle(transform.getPosition(), playerRect, transform.getRotation(), 0.1f);
	}

	void Player::fixedUpdate()
	{
		updateRays();
	}

}
