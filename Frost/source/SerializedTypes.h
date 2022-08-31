#pragma once
#include <functional>
#include <entt_snapshot/entt_snapshot.hpp>
#include <set>

// Serialized Types
#include <Transform.h>
#include <SpriteRenderer.h>
#include <cereal/cereal.hpp>

// Serialize SFML types
namespace sf
{
	// Vector
	template<typename Archive> void serialize(Archive& ar, sf::Vector2f & vec)
	{
		ar(cereal::make_nvp("x", vec.x), cereal::make_nvp("y", vec.y));
	}

	template<typename Archive> void serialize(Archive& ar, sf::Vector2i & vec)
	{
		ar(cereal::make_nvp("x", vec.x), cereal::make_nvp("y", vec.y));
	}

	template<typename Archive> void serialize(Archive& ar, sf::Vector2u & vec)
	{
		ar(cereal::make_nvp("x", vec.x), cereal::make_nvp("y", vec.y));
	}
}

namespace fl
{
	namespace SerializedTypes
	{
		constexpr std::string_view
			TRANSFORM_COMPONENT_NAME = "transform",
			SPRITERENDERER_COMPONENT_NAME = "sprite_renderer",
			RELATIONSHIP_COMPONENT_NAME = "relationship",
			PARENT_COMPONENT_NAME = "parent"
		;

		namespace
		{
			std::set<std::string_view> typeSet
			{
				TRANSFORM_COMPONENT_NAME,
				SPRITERENDERER_COMPONENT_NAME,
				RELATIONSHIP_COMPONENT_NAME,
				PARENT_COMPONENT_NAME
			};
		}

		// Register types for serialization
		inline void RegisterTypes()
		{
			snapshot::reflectComponent<Transform, TRANSFORM_COMPONENT_NAME>();
			snapshot::reflectComponent<SpriteRenderer, SPRITERENDERER_COMPONENT_NAME>();
			snapshot::reflectComponent<Relationship, RELATIONSHIP_COMPONENT_NAME>();
			snapshot::reflectComponent<Parent, PARENT_COMPONENT_NAME>();
		}

		inline snapshot::ShouldSerializePred Types()
		{
			return [](std::string const& name) { return typeSet.find(name) != typeSet.end(); };
		}
	}
}
