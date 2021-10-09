#pragma once
#include <Scene.h>
#include <json.hpp>
#include <uuid.h>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <string>

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
		JumpThroughLand = 1 << 5,
		All = Default | Player | Enemy | Collectible | Land | JumpThroughLand,
	};

	//Forward declarations
	struct scene;
	struct component;

	struct gameObject
	{
		fl::scene* ownerScene;

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
		//Parent is not a smart pointer because the child will never have to clean up the parent
		//The children are unique pointers because they are owned by the parent
		gameObject* parent;
		std::vector<std::unique_ptr<gameObject>> children;

		//Constructor/Destructor for non-serialized objects
		gameObject(fl::scene* scene = nullptr, gameObject* parent = nullptr, std::string name = "", Layer layer = Layer::Default, sf::Vector2f position = sf::Vector2f(0.f, 0.f), float rotation = 0.f, sf::Vector2f scale = sf::Vector2f(1.f, 1.f));
		
		//Sample json constructor
		gameObject(nlohmann::json json, fl::scene* scene = nullptr);

		//Remove copy and assign
		gameObject(const gameObject&) = delete;
		void operator=(const gameObject&) = delete;

		//Destructor
		virtual ~gameObject();

		//Custom init for object specific data
		virtual void customInit();

		//Move child from gameObject to another
		void moveChildTo(gameObject* child, gameObject* newGameObject);
		void moveChildTo(gameObject* child, scene* scene);
		void moveChildFrom(gameObject* child, gameObject* oldGameObject);
		void moveChildFrom(gameObject* child, scene* scene);

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
		nlohmann::json serializeBasic(std::string type = "basic");
		virtual nlohmann::json serialize();
		nlohmann::json serializeTransform();
		virtual sf::Transformable deserializeTransform(nlohmann::json json);
	};

	struct component
	{
		//The gameobject the component is on
		gameObject* owner;
		std::string name;
		bool enabled;
		virtual ~component();
		virtual void awake();
		virtual void start();
		virtual void update();
		virtual void preFixedUpdate();
		virtual void fixedUpdate();
	};
}
