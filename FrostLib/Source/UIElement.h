#pragma once
#include <uuid.h>
#include <SFML/Graphics.hpp>
#include <json.hpp>
#include <iostream>

namespace fl
{
	namespace UI
	{
		enum class ScalingType
		{
			Fractional,
			Distanced,
			Constant
		};

		enum class PositionType
		{
			Fractional,
			Constant
		};

		struct UIElement
		{
		protected:
			//These should not be accessed externally
			bool updatedDimensions;
			sf::Vector2f pixelPosition;
			sf::Vector2f pixelSize;

		public:
			uuids::uuid uuid;
			std::string name;
			sf::Vector2f anchor;
			sf::Vector2f position;
			sf::Vector2f size;
			sf::Color color;
			ScalingType scalingType;
			PositionType positionType;
			std::shared_ptr<UIElement> parent;
			std::vector <std::shared_ptr<UIElement>> children;

			UIElement();
			
			//If no parent, define parent as nullptr
			UIElement(UIElement* parentElement, const std::string elementName = "", ScalingType scaleType = ScalingType::Fractional, PositionType posType = PositionType::Fractional);

			//Create UIElement from json
			UIElement(nlohmann::json& json, std::vector<UIElement>& elementVector);

			//Function that can be overriden to initiate custom properties for specific UIElement types
			virtual void initCustomProperties();

			//Destructor, any needed cleanup should be performed here
			~UIElement();

			//No longer needed because uses of unique_ptr has been replaced by shared_ptr
				//UIElement(const UIElement&) = delete;
				//UIElement& operator= (const UIElement) = delete;

			//Adds a child to the current UIElement. Additionally sets *this to the parent
			void addChild(UIElement& child);

			//Invalidates the dimensions for the UIElement and all its children
			void invalidateDimensions();

			//Updates the pixel size of the UIElement
			virtual void updatePixelSize();

			//Updates the pixel position of the UIElement
			virtual void updatePixelPosition();

			//Gets the pixel size of the UIElement
			virtual sf::Vector2f getPixelSize();

			//Gets the pixel position of the UIElement
			virtual sf::Vector2f getPixelPosition();

			//Renders the UIElement to the specified window
			virtual void renderElement(sf::RenderWindow& output);

			nlohmann::json serialize();
		};
	}
}