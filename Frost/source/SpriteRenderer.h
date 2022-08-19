#include <ResourceMan.h>

#include <filesystem>

// sfml
#include <SFML/Graphics/Texture.hpp>

// json
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>

namespace fl
{
	enum class SortingLayer
	{
		Background = -1,
		Default = 0,
		Foreground = 1,
	};

	inline std::vector<std::filesystem::path> textureIDs;

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
		std::size_t textureID;
		sf::Texture* texture;
		SortingLayer layer;

		SpriteRenderer(std::filesystem::path texture = std::filesystem::path("common/null.png"), SortingLayer layer = SortingLayer::Default)
			: texture(ResourceMan::getTexture(texture)), layer(layer)
		{
			textureID = textureIDs.size();
			textureIDs.push_back(texture); 
		};

		// Serialization
		friend void to_json(nlohmann::json&, const SpriteRenderer&);
		friend void from_json(const nlohmann::json&, SpriteRenderer&);
	};
}