#include <gameObject.h>
#include <Debug.h>

namespace fl
{
	gameObject::gameObject(gameObject* parent, std::string name, Layer layer, sf::Vector2f position, float rotation, sf::Vector2f scale)
	{
		active = true;
		uuid = uuids::uuid_random_generator{}();
		if (parent)
		{
			parent->addChild(this);
			this->name != "" ? this->name = name : this->name = "gameObject " + std::to_string(this->parent.get()->children.size());
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

	gameObject::~gameObject()
	{
		if (parent) parent->unparentChild(this);
		fl::Debug::log("Destroying" + uuids::to_string(uuid));
		//Smart pointers and vectors will automatically be deleted ?
	}

	void gameObject::customInit()
	{}

	void gameObject::addChild(gameObject* child)
	{
		//Check this because child might not be fully constructed
		child->parent = std::make_shared<gameObject>(this);
		children.push_back(std::make_shared<gameObject>(child));
	}

	void gameObject::unparentChild(gameObject* child)
	{
		int index = -1;
		for (int x = 0; x < children.size(); x++)
		{
			if (children[x].get() == child)
			{
				index = x;
				break;
			}
		}
		if(index == -1) throw "Child not found in children vector";
		child->parent = nullptr;
		children.erase(children.begin() + index);
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

		json["position"].push_back(pos.x);
		json["position"].push_back(pos.y);
		json["rotation"] = rotation;
		json["scale"].push_back(scale.x);
		json["scale"].push_back(scale.y);

		return json;
	}

	nlohmann::json gameObject::serialize()
	{
		nlohmann::json json;
		json["name"] = name;
		json["uuid"] = uuids::to_string(uuid);
		json["layer"] = layer;
		json["active"] = active;
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
