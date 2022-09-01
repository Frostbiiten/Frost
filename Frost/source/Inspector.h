#pragma once
#include <Scene.h>
#include <entt/entt.hpp>
#include <AssetBrowser.h>

namespace fl
{
	namespace Editor
	{
		template <typename T>
		class EditAction
		{
			T& field;
			T oldValue; // Store old value for undo
			T newValue;

		public:
			EditAction(T& field, T newValue) : field(field), oldValue(field), newValue(newValue) {};
			void Process()
			{
				field = newValue;
			}
		};

		class Inspector
		{
			Scene& scene;
			entt::registry& sceneRegistry;

			AssetBrowser browser;

		public:
			Inspector (Scene& scene) : scene(scene), sceneRegistry(scene.GetRegistry()), browser("common") {}
			void Draw();
			void ProcessEditQueue();
		};
	}
}
