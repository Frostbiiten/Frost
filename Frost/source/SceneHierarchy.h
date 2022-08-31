#pragma once
#include <Scene.h>
#include <entt/entt.hpp>
#include <imgui.h>

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
			SceneHierarchy (Scene& scene) : scene(scene), selectedEntity(entt::null), sceneRegistry(scene.GetRegistry())
			{
				//ImGui::SetWindowSize("Hierarchy", ImVec2(225, 400));
				//ImGui::SetWindowPos("Hierarchy", ImVec2(10, 30));
			}
			void Draw();
		};

		inline SceneHierarchy* focusedInstance;
	}
}
