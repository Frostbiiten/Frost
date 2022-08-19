#include <Transform.h>
#include <entt/entt.hpp>
#include <SFML/System/Vector2.hpp>
#include <nlohmann/json.hpp>

namespace fl
{
    void to_json(nlohmann::json& j, const Transform& t)
    {
        j = nlohmann::json
        {
            {"position", {t.position.x, t.position.y}},
            {"pivot", {t.pivot.x, t.pivot.y}},
            {"depth", t.depth},
            {"scale", {t.scale.x, t.scale.y}},
            {"rotation", t.rotation}
        };
    }

    void from_json(const nlohmann::json& j, Transform& t)
    {
        // Array for vector2
        std::array<float, 2> vecArray;

        // Position
        j.at("position").get_to(vecArray);
        t.position.x = vecArray[0];
        t.position.y = vecArray[1];

        // Pivot
        j.at("pivot").get_to(vecArray);
        t.pivot.x = vecArray[0];
        t.pivot.y = vecArray[1];

        // Depth
        j.at("depth").get_to(t.depth);

        // Scale
        j.at("scale").get_to(vecArray);
        t.scale.x = vecArray[0];
        t.scale.y = vecArray[1];

        // Rotation
        j.at("rotation").get_to(t.rotation);
    }

    void to_json(nlohmann::json& j, const DirtyTransform& t) { j = nlohmann::json(); }
    void from_json(nlohmann::json& j, const DirtyTransform& t) { }
}