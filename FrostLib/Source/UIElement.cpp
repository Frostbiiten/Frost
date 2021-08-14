#include <UIElement.h>

#include <Debug.h>
#include <Utils.h>
#include <uuid.h>

namespace fl
{
	namespace UI
	{
		UIElement::UIElement()
		{
			uuids::uuid_random_generator gen;
			uuid = gen();
			name = "Element " + uuids::to_string(uuid);
			scalingType = ScalingType::Fractional;
			positionType = PositionType::Fractional;
			anchor = sf::Vector2f(0.5f, 0.5f);
			position = sf::Vector2f(0.5f, 0.5f);
			size = sf::Vector2f(0.2f, 0.2f);
			color = sf::Color::White;
			updatedDimensions = false;
			initCustomProperties();
		}

		UIElement::UIElement(UIElement* parentElement, const std::string elementName, ScalingType scaleType, PositionType posType)
		{
			uuids::uuid_random_generator gen;
			uuid = gen();
			if (parentElement)
			{
				parent.get()->addChild(*this);
				parent = std::make_shared<UIElement>(parentElement);
				elementName != "" ? name = elementName : name = "Element " + std::to_string(parent.get()->children.size());
			}
			else
			{
				name != "" ? name = elementName : name = uuids::to_string(uuid);
			}

			scalingType = scaleType;
			positionType = posType;
			anchor = sf::Vector2f(0.5f, 0.5f);
			position = sf::Vector2f(0.5f, 0.5f);
			size = sf::Vector2f(0.2f, 0.2f);
			color = sf::Color(sf::Color::White);
			updatedDimensions = false;
			initCustomProperties();
		}

		UIElement::UIElement(nlohmann::json& json, std::vector<UIElement>& elementVector)
		{
			//uuid
			uuid = uuids::uuid::from_string(json["uuid"].get<std::string>());

			//name
			name = json["name"].get<std::string>();

			//anchor
			std::array<float, 2> anchorArr = json["anchor"].get<std::array<float, 2>>();
			anchor.x = anchorArr[0];
			anchor.y = anchorArr[1];

			//position
			std::array<float, 2> positionArr = json["position"].get<std::array<float, 2>>();
			position.x = positionArr[0];
			position.y = positionArr[1];
			
			//size
			std::array<float, 2> sizeArr = json["size"].get<std::array<float, 2>>();
			size.x = sizeArr[0];
			size.y = sizeArr[1];

			//color
			std::array<unsigned char, 4> colorArr = json["color"].get<std::array<unsigned char, 4>>();
			color.r = colorArr[0];
			color.g = colorArr[1];
			color.b = colorArr[2];
			color.a = colorArr[3];

			//behavior enums
			scalingType = json["scaling_type"];
			positionType = json["position_type"];

			//children
			for (nlohmann::json& child : json["children"])
			{
				elementVector.push_back(UIElement(child, elementVector));
				addChild(elementVector[elementVector.size() - 1]);
			}

			updatedDimensions = false;
		}

		void UIElement::initCustomProperties()
		{
			fl::Debug::log("Init custom properties has not been overriden");
		}

		UIElement::~UIElement()
		{
			//Any cleanup
		}

		void UIElement::addChild(UIElement& child)
		{
			child.parent = std::make_shared<UIElement>(*this);
			children.push_back(std::make_shared<UIElement>(child));
		}

		void UIElement::invalidateDimensions()
		{
			updatedDimensions = false;
			for (size_t x = 0; x < children.size(); x++)
			{
				children[x].get()->invalidateDimensions();
			}
		}

		void UIElement::updatePixelSize()
		{
			sf::Vector2f parentSize = parent ? parent.get()->getPixelSize() : getWindowSize();
			
			switch (scalingType)
			{
			case ScalingType::Constant:
				pixelSize = size;
				break;
			case ScalingType::Fractional:
				pixelSize = multiply(size, parentSize);
				break;
			case ScalingType::Distanced:
				pixelSize = parentSize - multiply(size, 2);
				break;
			}
		}

		void UIElement::updatePixelPosition()
		{
			sf::Vector2f parentSize = parent ? parent.get()->getPixelSize() : getWindowSize();
			sf::Vector2f parentPos = parent ? parent.get()->getPixelPosition() : sf::Vector2f(0, 0);

			//Position types switch
			switch (positionType)
			{
			case PositionType::Constant:
				if(parent)
					pixelPosition = parent.get()->getPixelPosition() + position;
				else
					pixelPosition = position;
				break;
			case PositionType::Fractional:
				pixelPosition = parentPos + multiply(position, parentSize);
			}

			//Dealing with anchor
			pixelPosition -= multiply(pixelSize, anchor);
		}

		sf::Vector2f UIElement::getPixelSize()
		{
			if (!updatedDimensions)
			{
				updatePixelSize();
				updatePixelPosition();
				updatedDimensions = true;
			}

			return pixelSize;
		}

		sf::Vector2f UIElement::getPixelPosition()
		{
			if (!updatedDimensions)
			{
				updatePixelSize();
				updatePixelPosition();
				updatedDimensions = true;
			}

			return pixelPosition;
		}

		//this should be overridden
		void UIElement::renderElement(sf::RenderWindow& output)
		{
			sf::RectangleShape shape;
			shape.setSize(getPixelSize());
			shape.setPosition(getPixelPosition());
			shape.setFillColor(sf::Color::White);

			output.draw(shape);

			for (size_t x = 0; x < children.size(); x++)
			{
				children[x].get()->renderElement(output);
			}
		}

		nlohmann::json UIElement::serialize()
		{
			nlohmann::json j;
			std::string uuidStr = uuids::to_string(uuid);

			std::vector<nlohmann::json> serializedChildren(children.size());

			for (size_t x = 0; x < children.size(); x++)
			{
				serializedChildren[x] = children[x].get()->serialize();
			}

			try
			{
				j["uuid"] = uuidStr;
				j["name"] = name;
				j["anchor"] = std::array<float, 2> { anchor.x, anchor.y };
				j["position"] = std::array<float, 2> { position.x, position.y };
				j["size"] = std::array<float, 2> { size.x, size.y };
				j["color"] = std::array<unsigned char, 4> { color.r, color.g, color.b, color.a};
				j["scaling_type"] = scalingType;
				j["position_type"] = positionType;
				j["children"] = serializedChildren;
			}
			catch (nlohmann::json::type_error& err)
			{
				fl::Debug::log("JSON TYPE ERROR " + err.id + std::string(": ") + err.what());
			}

			return j;
		}
	}
}