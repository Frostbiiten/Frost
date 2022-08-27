#include <SpriteRenderer.h>

namespace fl
{
    void to_json(nlohmann::json& j, const SpriteRenderer& t)
    {
        j = nlohmann::json
        {
            {"layer", t.layer},
            {"id", ResourceMan::getTextureID(t.texture)},
        };
    }

    void from_json(const nlohmann::json& j, SpriteRenderer& t)
    {
        std::string hm = j.type_name();

        SortingLayer layer;
        j.at("layer").get_to(layer);

        std::string path;
        j.at("path").get_to(path);

        t = SpriteRenderer(std::filesystem::path(path), SortingLayer::Default);
    }
}