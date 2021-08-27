#include <json.hpp>
#include <uuid.h>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <string>
#pragma once

namespace fl
{
	//Enemy, player, etc
	enum class Layer
	{
		Default = 1 << 0,
		Player = 1 << 1,
		Enemy = 1 << 2,
		Collectible = 1 << 3,
		Land = 1 << 4,
		All = Default | Player | Enemy | Collectible | Land,
	};

	//Forward declaration
	struct component;

	struct gameObject
	{
		//Identifiers
		std::string name;
		uuids::uuid uuid;
		Layer layer;
		bool active;
		std::vector<std::unique_ptr<component>> components;

		//The transform of the object. This is always local
		sf::Transformable transform;
		sf::Transform localTransform;
		bool validLocalTransform;

		//Pointers to the parent and children
		std::shared_ptr<gameObject> parent;
		std::vector<std::shared_ptr<gameObject>> children;

		//Constructor/Destructor
		gameObject(gameObject* parent = nullptr, std::string name = "", Layer layer = Layer::Default, sf::Vector2f position = sf::Vector2f(0.f, 0.f), float rotation = 0.f, sf::Vector2f scale = sf::Vector2f(1.f, 1.f));
		~gameObject();

		//Custom init for object specific data
		virtual void customInit();

		//Add + Remove* child
		void addChild(gameObject* child);
		void unparentChild(gameObject* child);
		void updateLocalTransform();

		//Main functions
		virtual void draw(sf::Drawable& target); //may move this to a designated spriterenderer component
		virtual void awake();
		virtual void start();
		virtual void update();
		virtual void preFixedUpdate();
		virtual void fixedUpdate();

		//Serialize
		nlohmann::json serialize();
		nlohmann::json serializeTransform();
	};

	struct component
	{
		//The gameobject the component is on
		gameObject* owner;
		std::string name;
		bool enabled;
		virtual void awake();
		virtual void start();
		virtual void update();
		virtual void preFixedUpdate();
		virtual void fixedUpdate();
	};
}
