#include <SceneMan.h>
#include <AssetMan.h>
#include <fmt/core.h>
#include <Scene.h>
#include <Debug.h>

// For entt hashed string
#include <entt/core/hashed_string.hpp>
using entt::operator""_hs;

// Serialization
#include <SpriteRenderer.h>

namespace fl
{
	namespace SceneMan
	{
		Scene currentScene {"mainscene"};

		// Time management
		sf::Time deltaTime {};
		sf::Clock deltaTimeClock {};
		unsigned long long frameCount = 0;
		sf::Int32 fixedStepOffset = 0;

		// Drawing targets
		sf::RenderWindow* windowPtr;
		sf::RenderTarget* bufferPtr;

		// Forward declaration
		void SaveScene(std::string_view sceneName);

		void Reset(sf::RenderWindow& window, sf::RenderTarget& buffer)
		{
			// Scene management
			SceneMan::ClearScene();

			TEST(SceneMan::LoadScene("test_scene", false););

			SceneMan::Awake();
			SceneMan::Start();

			TEST(SceneMan::SaveScene("test_scene_save"););

			// Time management
			deltaTimeClock.restart();
			deltaTime = sf::Time{};
			fixedStepOffset = 0;
			frameCount = 0;

			windowPtr = &window;
			bufferPtr = &buffer;
		}

		// Loads scene data from a file
		void LoadScene(std::string_view sceneName, bool additive)
		{
			std::string sceneData;
			if (AssetMan::readFile(std::filesystem::path(fmt::format("scenes/{0}/{0}.scene", sceneName)), sceneData))
			{
				currentScene.Load(sceneData, additive);
			}
			else
			{
				Debug::log()->error("Could not load scene \'{}\'!", sceneName);
			}
		}

		// Loads scene data from a file
		void SaveScene(std::string_view sceneName)
		{
			std::stringstream sceneData;
			cereal::JSONOutputArchive output { sceneData };
			snapshot::OutputArchive archive { output };
			currentScene.Serialize(archive);

			Debug::log()->info(sceneData.str());

			if (AssetMan::writeFile(std::filesystem::path(fmt::format("scenes/{0}/{0}.scene", sceneName)), sceneData.str()))
			{
				Debug::log()->info("Successfully saved scene \'{}\'!", sceneName);
			}
			else
			{
				Debug::log()->error("Could not save scene \'{}\'!", sceneName);
			}
		}

		void ClearScene()
		{
			currentScene.Clear();
		}

		void Awake()
		{
			currentScene.Awake();
		}

		void Start()
		{
			currentScene.Start();
		}

		void FixedUpdate()
		{
			currentScene.FixedUpdate();
		}

		void ProcessFixedUpdate()
		{
			fixedStepOffset += deltaTime.asMilliseconds();

			while (fixedStepOffset > fixedTimestep)
			{
				fixedStepOffset -= fixedTimestep;
				FixedUpdate();
			}
		}

		void Update()
		{
			deltaTime = deltaTimeClock.restart();
			currentScene.Update(deltaTime.asMicroseconds());
		}

		void Draw()
		{
			currentScene.Draw(*windowPtr);
		}

		void Tick()
		{
			++frameCount;
			ProcessFixedUpdate();
			Update();
			Draw();
		}
	}
}

