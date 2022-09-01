#include <SceneMan.h>
#include <AssetMan.h>
#include <fmt/core.h>
#include <Scene.h>
#include <Debug.h>
#include <imgui.h>

// Editor
#include <SceneHierarchy.h>
#include <MenuBar.h>

// For entt hashed string
#include <entt/core/hashed_string.hpp>
using entt::operator""_hs;

// Serialization
#include <SpriteRenderer.h>

// Editor
#include <Compression.h>
#include <Inspector.h>
#include <AssetBrowser.h>

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

		// Editor
		Editor::SceneHierarchy hierarchy{currentScene};
		Editor::Inspector inspector{currentScene};
		Editor::AssetBrowser browser {"common"};

		// Forward declaration
		void SaveScene(std::string_view sceneName);

		void Reset(sf::RenderWindow& window, sf::RenderTarget& buffer)
		{
			Compression::Test();

			// Scene management
			SceneMan::ClearScene();

			TEST(SceneMan::LoadScene("test_scene_save", false););

			SceneMan::Awake();
			SceneMan::Start();

			//TEST(SceneMan::SaveScene("test_scene_save"););

			// Time management
			deltaTimeClock.restart();
			deltaTime = sf::Time{};
			fixedStepOffset = 0;
			frameCount = 0;

			windowPtr = &window;
			bufferPtr = &buffer;
		}

		// Loads scene data from a file
		// TODO: implement additive loading
		void LoadScene(std::string_view sceneName, bool additive)
		{
			std::stringstream sceneData;
			if (AssetMan::readFile(std::filesystem::path(fmt::format("scenes/{0}/{0}.scene", sceneName)), sceneData))
			{
				nlohmann::json sceneJson = nlohmann::json::parse(sceneData);

				// Load textureIDS
				ResourceMan::loadTextureIDs(sceneJson["textureIDs"], additive);

				// Load registry part of json
				std::stringstream registry (sceneJson["registry"].get<std::string>()); // HACK: I imagine this is very inefficient
				cereal::JSONInputArchive registryInput { registry };
				snapshot::InputArchive archive { registryInput };

				// Finally deserialize
				currentScene.Deserialize(archive, additive);
			}
			else
			{
				Debug::log()->error("Could not load scene \'{}\'!", sceneName);
			}
		}

		// Loads scene data from a file
		void SaveScene(std::string_view sceneName)
		{
			std::stringstream registryData;
			nlohmann::json sceneJson;

			{
				// Serialize registry
				cereal::JSONOutputArchive registryOutput { registryData };
				snapshot::OutputArchive archive { registryOutput };
				currentScene.Serialize(archive);
			}

			sceneJson["registry"] = registryData.str(); // Just store raw string
			sceneJson["textureIDs"] = ResourceMan::getTextureIDMap();

			Debug::log()->info(sceneJson.dump());

			if (AssetMan::writeFile(std::filesystem::path(fmt::format("scenes/{0}/{0}.scene", sceneName)), sceneJson.dump()))
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
			inspector.ProcessEditQueue();
		}

		void Draw()
		{
			currentScene.Draw(*windowPtr); // native res drawing
			//currentScene.Draw(*bufferPtr); // pixel drawing

			hierarchy.Draw();
			inspector.Draw();
			browser.Draw();
			Editor::MenuBar::DrawBar();
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

