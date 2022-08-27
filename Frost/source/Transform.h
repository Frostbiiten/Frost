#pragma once
#include <entt/entt.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <nlohmann/json_fwd.hpp>

// cereal
#include <cereal/cereal.hpp>

namespace fl
{
	struct Transform
	{
		friend struct Scene;

		// Position
		sf::Vector2f position;
		sf::Vector2f pivot;
		float depth;

		sf::Vector2f scale;
		float rotation;

		// Constructor
		Transform(sf::Vector2f position = sf::Vector2f(0, 0), sf::Vector2f pivot = sf::Vector2f(0, 0), float rotation = 0.f, float depth = 0.f, sf::Vector2f scale = sf::Vector2f(1, 1))
		: position(position), pivot(pivot), rotation(rotation), depth(depth), scale(scale) {};

		// Deconstructor
		~Transform() {};

		// Serialization
		friend void to_json(nlohmann::json&, const Transform&);
		friend void from_json(const nlohmann::json&, Transform&);

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar
			(
				cereal::make_nvp("position", position),
				cereal::make_nvp("pivot", pivot),
				cereal::make_nvp("scale", scale),
				cereal::make_nvp("rotation", rotation),
				cereal::make_nvp("depth", depth)
			);
		}

	protected:
		// Global transform
		sf::Transform globalTransform;
	};

	// Dirtied transform flag
	// Added to an entity when its transfrom is changed
	struct DirtyTransform
	{
		DirtyTransform() {};
		~DirtyTransform() {};

		friend void to_json(nlohmann::json&, const Transform&);
		friend void from_json(const nlohmann::json&, Transform&);

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar ();
		}
	};

	struct Parent
	{
		// Reference to parent entity
		entt::entity entity {entt::null};

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar (entity);
		}
	};

	struct Relationship
	{
		std::size_t children {}; // Number of children
		entt::entity firstChild {entt::null}; // First child
		entt::entity next {entt::null}; // Next sibling
		entt::entity prev {entt::null}; // Previous sibling

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar (children, firstChild, next, prev);
		}
	};
}
