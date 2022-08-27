#include <ResourceMan.h>

#include <filesystem>

// sfml
#include <SFML/Graphics/Texture.hpp>

// json
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>

// cereal
#include <cereal/cereal.hpp>

namespace fl
{
	enum class SortingLayer
	{
		Background,
		Default,
		Foreground
	};

	template <class Archive>
	std::string save_minimal(Archive const&, SortingLayer const& obj)
	{
		switch (obj)
		{
			case SortingLayer::Background: return "background";
			case SortingLayer::Default: return "default";
			case SortingLayer::Foreground: return "foreground";
		}

		// Default if invalid
		return "default";
	}

	template <class Archive>
	void load_minimal(Archive const &, SortingLayer & obj, std::string const & value)
	{
		if (value == "background") obj = SortingLayer::Background;
		else if (value == "default") obj = SortingLayer::Default;
		else if (value == "foreground") obj = SortingLayer::Foreground;
		else obj = SortingLayer::Default;
	}


	/*
	NLOHMANN_JSON_SERIALIZE_ENUM(SortingLayer,
	{
		{Background, "bg"},
		{Midground, "mg"},
		{Foreground "fg"},
	})
	*/

	struct SpriteRenderer
	{
		sf::Texture* texture;
		SortingLayer layer;

		SpriteRenderer(std::filesystem::path texture = std::filesystem::path("common/null.png"), SortingLayer layer = SortingLayer::Default) :
			texture(ResourceMan::getTexture(texture)), layer(layer) {};

		// Serialization
		friend void to_json(nlohmann::json&, const SpriteRenderer&);
		friend void from_json(const nlohmann::json&, SpriteRenderer&);

		template<class Archive>
		void serialize(Archive& ar)
		{
			// Sprites that share the same texture will share the same texture pointer address, use this to serialize
			ar (cereal::make_nvp("texture_id", ResourceMan::getTextureID(texture)), cereal::make_nvp("layer", layer));
		}
	};
}