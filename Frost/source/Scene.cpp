#include <Scene.h>
#include <Transform.h>
#include <SFML/Graphics/Transformable.hpp>
#include <queue>
#include <nlohmann/json.hpp>
#include <Debug.h>
#include <SceneArchive.h>

#include <AssetMan.h>
#include <SpriteRenderer.h>

// entt
#include <entt/entt.hpp>
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

namespace fl
{
	Scene::Scene(std::string name)
	{
		this->name = name;
		registry = entt::registry();
	}

	void Scene::Load(std::string_view data, bool additive)
	{
		//nlohmann::json j = nlohmann::json::from_msgpack(data);
	}
	void Scene::Serialize(SceneSaveArchive& archive)
	{
		entt::basic_snapshot snapshot(registry);
		snapshot.entities(archive).component<Transform, SpriteRenderer>(archive);
		archive.Close();
	}

	entt::entity Scene::CreateEntity()
	{
		return registry.create();
	}
	void Scene::DestroyEntity(entt::entity entity)
	{
		registry.destroy(entity);
	}
	void Scene::Clear()
	{
		registry.clear();
	}

	// Transforms
	void Scene::UpdateGlobalTransform(Transform& localTransform, const sf::Transform& parentTransform)
	{
		// Get local transform
		sf::Transformable t {};
		t.setOrigin(localTransform.pivot);
		t.setPosition(localTransform.position);
		t.setRotation(localTransform.rotation);
		t.setScale(localTransform.scale);

		// Combine with parent transform to update global transform
		localTransform.globalTransform = t.getTransform() * parentTransform;
	}
	void Scene::CleanTransform(sf::Transform& parentTransform, entt::entity root)
	{
		// Stores the gloabl transform of the respective entities
		std::map<entt::entity, sf::Transform> transformMap;

		// Set unknown parent to default parent transform
		transformMap[entt::null] = parentTransform;

		// Queue of entities to clean
		// Pair first is entity, second is parent
		std::queue<std::pair<entt::entity, entt::entity>> cleanQueue;

		// Add first entity to queue
		cleanQueue.push(std::make_pair(root, entt::null));

		while (!cleanQueue.empty())
		{
			// Extract first pair from queue, get entity and parent
			auto queueFront = cleanQueue.front();
			auto currentEntity = queueFront.first;
			auto currentParent = queueFront.second;
			cleanQueue.pop();

			// Update current transform
			Transform* currentTransform = registry.try_get<Transform>(currentEntity);
			if (currentTransform) UpdateGlobalTransform(*currentTransform, transformMap[currentParent]);

			// Add next sibling & first child to queue if possible
			Relationship* currentRelationship = registry.try_get<Relationship>(currentEntity);
			if (currentRelationship)
			{
				if (currentRelationship->next != entt::null) cleanQueue.push(std::make_pair(currentRelationship->next, currentParent));

				// Check if current entity has any children
				if (currentRelationship->firstChild != entt::null)
				{
					// Parent of child (because this transform may be null)
					auto p = currentParent;

					// If so, this entity's transform must be stored as it will later be referenced as a parent
					if (currentTransform)
					{
						transformMap[currentEntity] = currentTransform->globalTransform;
						p = currentEntity;
					}

					// Add first child to queue, it will add its next respective sibling to the queue when it is processed later
					cleanQueue.push(std::make_pair(currentRelationship->firstChild, p));
				}
			}

			// Remove DirtyTransform if any
			if (registry.any_of<DirtyTransform>(currentEntity)) registry.remove<DirtyTransform>(currentEntity);
		}
	}
	void Scene::UpdateTransforms()
	{
		// Update dirtied transforms
		auto dirtyTransformView = registry.view<DirtyTransform, Transform, Relationship>();

		for (auto entity : dirtyTransformView)
		{
			// Default identity transform
			sf::Transform rootTransform = sf::Transform::Identity;

			// Set root reference to parent if any
			auto parent = registry.try_get<Parent>(entity);
			if (parent && parent->entity != entt::null)
			{
				auto t = registry.try_get<Transform>(parent->entity);
				if (t) rootTransform = t->globalTransform;
			}

			CleanTransform(rootTransform, entity);

			// Transform is no longer dirty
			registry.remove<DirtyTransform>(entity);
		}
	}

	int total = 1000;
	void Scene::Awake()
	{
		std::string output;
		AssetMan::readFile(std::filesystem::path("scenes/test_scene.scene"), output);

		/*
		Debug::log()->info(registry.size());
		SceneLoadArchive archive{output};
		entt::basic_snapshot_loader<entt::entity> l {registry};
		l.entities(archive).component<Transform, SpriteRenderer>(archive);
		Debug::log()->info(registry.size());
		//*/

		///*
		for (int x = 0; x < total; ++x)
		{
			entt::entity e = registry.create();
			auto& t = registry.emplace<Transform>(e);

			t = Transform
			(
				sf::Vector2f(0.f, (x - total / 2) * (0.f / total)), // position
				sf::Vector2f(0.f, 0.f), // pivot
				0, //x * (360.f / total), // rotation
				0.f, sf::Vector2f(5.f, 0.5f) // scale
			);

			auto& r = registry.emplace<SpriteRenderer>(e);
			r = SpriteRenderer();
		}
		//*/

		///*
		SceneSaveArchive arc{};
		Serialize(arc);
		std::string test;

		try
		{
			nlohmann::json::to_msgpack(arc.root, test);
		}
		catch (nlohmann::json::type_error e)
		{
			Debug::log()->error("{}: {}", e.id, e.what());
		}

		AssetMan::writeFile(std::filesystem::path("scenes/test_scene.scene"), test, false);
		//AssetMan::writeFile(std::filesystem::path("scenes/test_scene2.scene"), arc.AsString(0), false);
		//*/
	}
	void Scene::Start()
	{

	}

	double time = 0;

	// Loops
	void Scene::Update(sf::Int64 deltaTime)
	{
		auto renderers = registry.view<Transform, SpriteRenderer>();

		double freq = 0.02;
		time += deltaTime * 0.00008f * freq;

		int index = 0;
		float i;
		for (auto entity : renderers)
		{
			i = index / (float)total * 360;
			Transform& transform = renderers.get<Transform>(entity);
			transform.position = sf::Vector2f(std::cos(time + (i) * 0.1f) * 30.f, std::sin(time + (i) * freq) * 30.f);

			//transform.rotation += (deltaTime * 0.0001f);
			transform.depth = (std::cos(time + (i) * freq) + 0.5) * 100;

			++index;
		}

		//Debug::log()->info("{} ms", deltaTime / 1000.0);
		// Logic
		UpdateTransforms();
	}
	void Scene::FixedUpdate()
	{
		// Logic
		UpdateTransforms();
	}
	void Scene::Draw(sf::RenderTarget& target)
	{
		sf::Sprite sprite {};

		///*
		registry.sort<Transform>([](const auto& lhs, const auto& rhs) {return lhs.depth < rhs.depth; });
		//registry.sort<Transform, SpriteRenderer>();
		registry.sort<SpriteRenderer, Transform>();
		//*/

		// Update dirtied transforms
		auto renderers = registry.view<Transform, SpriteRenderer>();

		for (auto entity : renderers)
		{
			auto& spriteRenderer = renderers.get<SpriteRenderer>(entity);
			auto& transform = renderers.get<Transform>(entity);
			sprite.setTexture(*spriteRenderer.texture);
			sprite.setOrigin(transform.pivot);
			sprite.setScale(transform.scale);
			sprite.setRotation(transform.rotation);
			sprite.setPosition(transform.position);
			sprite.setColor(sf::Color(100 + transform.depth, 100 + transform.depth, 100 + transform.depth));

			//Debug::log()->info("{}", x);

			target.draw(sprite);
		}
	}

	void Scene::DumpRegistry()
	{
	}
}