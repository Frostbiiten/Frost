#include <SpriteAnimator.h>
#include <MathUtil.h>

namespace fl
{
	Animation::Animation(){}

	Animation::Animation(nlohmann::json json)
	{
		clampMode = json["clampMode"];
		frameDuration = json["frameDuration"];
		animation = json["frames"].get<std::vector<int>>();
	}

	SpriteAnimator::SpriteAnimator(nlohmann::json json, sf::Sprite& sprite)
	{
		//Create rects from the json
		createRects(json["textures"]);
		createAnimations(json["textures"]);
		this->sprite = &sprite;
	}

	void SpriteAnimator::createAnimations(nlohmann::json json)
	{
		for (auto& entry : json)
		{
			animations.clear();
			animations.insert(std::make_pair(entry["name"], Animation(entry)));
		}
	}

	void SpriteAnimator::createRects(int frames, sf::Vector2i frameSize)
	{
		sf::Vector2i pos;
		frameRects.resize(frames);
		for (int x = 0; x < frames; x++)
		{
			frameRects[x] = sf::IntRect(pos, frameSize);
			pos.x += frameSize.x;
			if (pos.x > sprite->getTexture()->getSize().x) pos.y += frameSize.y;
		}
	}

	void SpriteAnimator::createRects(nlohmann::json json)
	{
		frameRects.resize(json["images"].size());
		int index = 0;
		for (auto& entry : json["images"])
		{
			frameRects[index] = sf::IntRect(sf::Vector2i(entry["x"], entry["y"]), sf::Vector2i(entry["w"], entry["h"]));
			index++;
		}
	}

	void SpriteAnimator::tick(float deltaTime)
	{
		//Clamp animation timer to the range of the current animation
		animationTimer = Math::clamp(0, currentAnimation->animation.size() - currentAnimation->frameDuration, animationTimer, currentAnimation->clampMode);

		//Get current frame of animation (local to sheet)
		int currentFrame = animationTimer / currentAnimation->frameDuration;
		currentFrame = currentAnimation->animation[currentFrame];

		//Sets the rect of the sprite
		sprite->setTextureRect(frameRects[currentFrame]);

		//Add tick at end of code to retain first frame
		animationTimer += deltaTime * speedMultiplier;
	}

	void SpriteAnimator::playAnimation(std::string name)
	{
		//Check if animation exists in map
		if (animations.count(name))
		{
			//Resets animation timer to start at beginning of animation
			animationTimer = 0.f;
			//Set pointer to the found animation
			currentAnimation = &animations[name];
		}
	}

	void SpriteAnimator::setAnimationSpeed(float speed)
	{
		speedMultiplier = speed;
	}
}
