#pragma once
#include <string_view>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <filesystem>

namespace fl
{
	namespace SceneMan
	{
		//Time: the timestep is in milliseconds
		inline constexpr int fixedTimestep = 16;

		// Resets time management variables
		void Reset(sf::RenderWindow& window, sf::RenderTarget& buffer);

		void SaveScene(std::string_view sceneName);
		void LoadScene(std::string_view sceneName, bool additive = false);
		void LoadScene(std::filesystem::path path, bool additive = false);
		void ClearScene();

		void Awake(); // Called before anything is loaded
		void Start(); // Called after everything is loaded
		void Update(); // Called every frame
		void FixedUpdate(); // Called every fixed step
		void Draw(); // Called every fixed step

		// To be called every frame
		// Manages update, fixedupdate and drawing
		void Tick();
	}
}
