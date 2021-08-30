#include <gameObject.h>
#include <Debug.h>

namespace fl
{
	gameObject::gameObject(fl::scene* scene, gameObject* parent, std::string name, Layer layer, sf::Vector2f position, float rotation, sf::Vector2f scale)
	{
		active = true;
		this->ownerScene = scene;
		uuid = uuids::uuid_random_generator{}();
		if (parent)
		{
			parent->moveChildFrom(this, scene);
			this->name != "" ? this->name = name : this->name = "gameObject " + std::to_string(this->parent->children.size());
		}
		else
		{
			this->name != "" ? this->name = name : this->name = uuids::to_string(uuid);
		}

		this->layer = layer;

		transform.setPosition(position);
		transform.setRotation(rotation);
		transform.setScale(scale);

		customInit();

		validLocalTransform = false;
	}

	gameObject::gameObject(nlohmann::json json, fl::scene* scene)
	{
		name = json["name"];
		uuid = uuids::uuid::from_string(json["uuid"].get<std::string>());
		layer = json["layer"];
		active = json["active"];
		transform = deserializeTransform(json["transform"]);

		for (auto& child : json["children"])
		{
			scene->createGameObject(child, this);
		}
	}

	gameObject::~gameObject()
	{
		if (parent) parent->unparentChild(this);
		fl::Debug::log("Destroy gameobject " + uuids::to_string(uuid));
	}

	void gameObject::customInit()
	{}

	void gameObject::moveChildTo(gameObject* child, gameObject* newGameObject)
	{
		auto foundChild = std::find_if(std::begin(children), std::end(children),
			[&](std::unique_ptr<gameObject> const& obj) { return (*obj).uuid == child->uuid; });

		if (foundChild == children.end())
		{
			throw std::invalid_argument(Formatter() << "Child \"" << child->name <<
				'(' << uuids::to_string(child->uuid) << ')' << " Was not found in " <<
				name << '(' << uuids::to_string(uuid) << ')');
			return;
		}

		child->parent = newGameObject;
		newGameObject->children.push_back(std::move(*foundChild));
		children.erase(foundChild);
	}

	void gameObject::moveChildTo(gameObject* child, scene* scene)
	{
		auto foundChild = std::find_if(std::begin(children), std::end(children),
			[&](std::unique_ptr<gameObject> const& obj) { return (*obj).uuid == child->uuid; });

		if (foundChild == children.end())
		{
			throw std::invalid_argument(Formatter() << "Child \"" << child->name <<
				'(' << uuids::to_string(child->uuid) << ')' << " Was not found in " <<
				name << '(' << uuids::to_string(uuid) << ')');
			return;
		}

		child->parent = nullptr;
		scene->gameObjects.push_back(std::move(*foundChild));
		children.erase(foundChild);
	}

	void gameObject::moveChildFrom(gameObject* child, gameObject* oldGameObject)
	{
		auto foundChild = std::find_if(std::begin(oldGameObject->children), std::end(oldGameObject->children),
			[&](std::unique_ptr<gameObject> const& obj) { return (*obj).uuid == child->uuid; });

		if (foundChild == oldGameObject->children.end())
		{
			throw std::invalid_argument(Formatter() << "Child \"" << child->name <<
				'(' << uuids::to_string(child->uuid) << ')' << " Was not found in " <<
				oldGameObject->name << '(' << uuids::to_string(oldGameObject->uuid) << ')');
			return;
		}

		child->parent = this;
		children.push_back(std::move(*foundChild));
		oldGameObject->children.erase(foundChild);
	}

	void gameObject::moveChildFrom(gameObject* child, scene* scene)
	{
		auto foundChild = std::find_if(std::begin(scene->gameObjects), std::end(scene->gameObjects),
			[&](std::unique_ptr<gameObject> const& obj) { return (*obj).uuid == child->uuid; });

		if (foundChild == scene->gameObjects.end())
		{
			throw std::invalid_argument(Formatter() << "Child \"" << child->name <<
				'(' << uuids::to_string(child->uuid) << ')' << " Was not found in " <<
				scene->sceneName);
			return;
		}

		child->parent = this;
		children.push_back(std::move(*foundChild));
		scene->gameObjects.erase(foundChild);		
	}

	void gameObject::unparentChild(gameObject* child)
	{
		auto foundChild = std::find_if(std::begin(children), std::end(children),
			[&](std::unique_ptr<gameObject> const& obj) { return (*obj).uuid == child->uuid; });

		if (foundChild == children.end())
		{
			throw std::invalid_argument(Formatter() << "Child \"" << child->name <<
				'(' << uuids::to_string(child->uuid) << ')' << " Was not found in " <<
				name << '(' << uuids::to_string(uuid) << ')');
			return;
		}
		
		child->parent = nullptr;
		children.erase(foundChild);
	}

	void gameObject::updateLocalTransform()
	{
		if (parent)
			localTransform = transform.getTransform() * parent->transform.getTransform();
		else
			localTransform = transform.getTransform();
		validLocalTransform = true;
	}

	void gameObject::awake()
	{
		for (auto& element : components)
			if(element->enabled) element->awake();
	}

	void gameObject::start()
	{
		for (auto& element : components)
			if(element->enabled) element->start();
	}

	void gameObject::update()
	{
		for (auto& element : components)
			if(element->enabled) element->update();
	}

	void gameObject::preFixedUpdate()
	{
		for (auto& element : components)
			if(element->enabled) element->preFixedUpdate();
	}

	void gameObject::fixedUpdate()
	{
		for (auto& element : components)
			if(element->enabled) element->fixedUpdate();
	}

	void gameObject::draw(sf::Drawable& target)
	{
		if (!validLocalTransform) { updateLocalTransform(); }
	}

	nlohmann::json gameObject::serializeTransform()
	{
		nlohmann::json json;
		sf::Vector2f pos = transform.getPosition();
		float rotation = transform.getRotation();
		sf::Vector2f scale = transform.getPosition();
		sf::Vector2f origin = transform.getOrigin();

		json["position"].push_back(pos.x);
		json["position"].push_back(pos.y);
		json["rotation"] = rotation;
		json["scale"].push_back(scale.x);
		json["scale"].push_back(scale.y);
		json["origin"].push_back(origin.x);
		json["origin"].push_back(origin.y);

		return json;
	}

	sf::Transformable gameObject::deserializeTransform(nlohmann::json json)
	{
		sf::Transformable tempTransform;

		tempTransform.setPosition(sf::Vector2f(json["position"][0], json["position"][0]));
		tempTransform.setRotation(json["rotation"]);
		tempTransform.setScale(sf::Vector2f(json["scale"][0], json["scale"][0]));
		tempTransform.setOrigin(sf::Vector2f(json["origin"][0], json["origin"][0]));

		return tempTransform;
	}

	nlohmann::json gameObject::serializeBasic(std::string type)
	{
		nlohmann::json json;
		json["name"] = name;
		json["uuid"] = uuids::to_string(uuid);
		json["layer"] = layer;
		json["active"] = active;
		return json;
	}

	nlohmann::json gameObject::serialize()
	{
		//Start with serializing the most important parts of a gameObject
		nlohmann::json json = serializeBasic();
		json["transform"] = serializeTransform();

		for (auto& child : children)
		{
			json["children"].push_back(child->serialize());
		}

		return json;
	}

	void component::awake()
	{
		fl::Debug::log("Awake method on component \"" + name + "\" has not been overriden in gameobject \"" + owner->name);
	}

	void component::start()
	{
		fl::Debug::log("Start method on component \"" + name + "\" has not been overriden in gameobject \"" + owner->name);
	}

	void component::update()
	{
		fl::Debug::log("Update method on component \"" + name + "\" has not been overriden in gameobject \"" + owner->name);
	}

	void component::preFixedUpdate()
	{}

	void component::fixedUpdate()
	{
		fl::Debug::log("Fixedupdate method on component \"" + name + "\" has not been overriden in gameobject \"" + owner->name);
	}
}
