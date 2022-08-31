#pragma once
#include <Scene.h>
#include <entt/entt.hpp>

namespace fl
{
	namespace Editor
	{
		class SceneHierarchy
		{
			Scene& scene;
			entt::registry& sceneRegistry;
			void DrawEntity(entt::entity entity, std::size_t depth);

		public:
			entt::entity selectedEntity;
			SceneHierarchy (Scene& scene) : scene(scene), selectedEntity(entt::null), sceneRegistry(scene.GetRegistry()) {}
			void Draw();
		};

		inline SceneHierarchy* focusedInstance;
	}
}
