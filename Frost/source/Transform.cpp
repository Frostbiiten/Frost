#include <Transform.h>
#include <entt/entt.hpp>
#include <SFML/System/Vector2.hpp>
#include <nlohmann/json.hpp>
#include <Debug.h>

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

    // Transform

    void DumpEntity(entt::registry& registry, entt::entity entity, std::size_t depth)
    {
        Debug::log()->info("{} Entity {}", std::string(depth, '\t'), (std::size_t)entity);
		Relationship* relationship = registry.try_get<Relationship>(entity);
		if (relationship)
		{
			entt::entity currentChild = relationship->children ? relationship->firstChild : entt::null;
			for (std::size_t x = 0; x < relationship->children; ++x)
			{
				DumpEntity(registry, currentChild, depth + 1);

				Relationship* childRelation = registry.try_get<Relationship>(currentChild);
				if (childRelation && childRelation->next != entt::null)
				{
					currentChild = childRelation->next;
				}
			}
		}
    }

    void DumpHierarchy(entt::registry& registry)
    {
		registry.each([&](auto entity) {
			if (!registry.any_of<Parent>(entity))
			{
				DumpEntity(registry, entity);
			}
		});
    }

	void SetParent(entt::registry& registry, entt::entity entity, entt::entity parent)
	{
		Parent& parentComponent = registry.get_or_emplace<Parent>(entity);
		Relationship& relationship = registry.get_or_emplace<Relationship>(entity);

		// Unlink from old parent and siblings
		if (parentComponent.entity != entt::null)
		{
			Relationship& oldParentRelation = registry.get<Relationship>(parentComponent.entity);

			// Fix parent relationships
			if (oldParentRelation.firstChild == entity) oldParentRelation.firstChild = relationship.next;
			--oldParentRelation.children;

			// Seal gap in relationship
			if (relationship.prev != entt::null) registry.get<Relationship>(relationship.prev).next = relationship.next;
			if (relationship.next != entt::null) registry.get<Relationship>(relationship.next).prev = relationship.prev;
		}

		if (parent != entt::null)
		{
			// Link to new parent and siblings
			Relationship& newParentRelation = registry.get_or_emplace<Relationship>(parent);

			// Set new parent's first child's prev to this
			if (newParentRelation.children)
			{
				Relationship* firstChildRelation = registry.try_get<Relationship>(newParentRelation.firstChild);
				if (firstChildRelation) firstChildRelation->prev = entity;
			}

			// Set next to old first child
			relationship.next = newParentRelation.firstChild;

			// Set new parent's firstchild to this
			newParentRelation.firstChild = entity;
			++newParentRelation.children;

			// Set new parent
			parentComponent.entity = parent;
		}
		else
		{
			registry.remove<Parent>(entity);
		}
	}

    void to_json(nlohmann::json& j, const DirtyTransform& t) { j = nlohmann::json(); }
    void from_json(nlohmann::json& j, const DirtyTransform& t) { }
}