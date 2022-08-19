#pragma once
#include <entt/entt.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <nlohmann/json.hpp>
#include <Transform.h>
#include <SceneArchive.h>

namespace fl
{
	struct Scene
	{
		// Name identifier
		std::string name;

		// Constructor & Deconstructor
		Scene(std::string name);
		~Scene() {};

		// Loading & Saving
		void Load(std::string_view data, bool additive);
		void Serialize(SceneSaveArchive& archive);

		entt::entity CreateEntity();
		void DestroyEntity(entt::entity);
		void Clear();

		void Awake();
		void Start();

		// Loop functions
		void Update(sf::Int64 deltaTime);
		void FixedUpdate();
		void Draw(sf::RenderTarget& target);

		void DumpRegistry();

	private:

		// Transform management functions
		void UpdateGlobalTransform(Transform& localTransform, const sf::Transform& parentTransform);
		void CleanTransform(sf::Transform& parentTransform, entt::entity root);
		void UpdateTransforms();

		// Registry of entities
		entt::registry registry;
	};
}