#pragma once
#include <MathUtil.h>
#include <SFML/Graphics.hpp>
#include <json.hpp>
#include <set>

namespace fl
{
	struct Animation
	{
		std::vector<int> animation; //The frames of the animation
		float frameDuration = 24.f; //How long a frame lasts in ms
		Math::ClampMode clampMode = Math::ClampMode::Loop; //Defines how an animation behaves when it reaches its last frame

		Animation();
		Animation(nlohmann::json json);
	};

	class SpriteAnimator
	{
		std::vector<sf::IntRect> frameRects;
		std::map<std::string, Animation> animations;
		Animation* currentAnimation;
		float animationTimer = 0.f;
		float speedMultiplier = 1.f;

	public:
		//Initialize from a sprite sheet and frames
		SpriteAnimator(nlohmann::json json, sf::Sprite& sprite);

		//Pointer to the image to be animated
		sf::Sprite* sprite;

		//Creates an array of frames from a frame count and the size of each frame.
		void createRects(int frames, sf::Vector2i frameSize);

		//Creates a vector of frames from a frame count and the size of each frame. Order is left to right, top to bottom
		void createRects(nlohmann::json json);

		//Creates animations based on frames specified from json
		void createAnimations(nlohmann::json json);

		//Tick the current frame based on the animation playing and time since last frame (deltaTime)
		//This should be called every frame animation will be updated
		void tick(float deltaTime);

		//Play an animation
		void playAnimation(std::string name);

		//Set the speed multiplier of an animation. This can be set to zero to pause an animation or even negative to rewind
		void setAnimationSpeed(float speed);
	};
}
