#include <imgui.h>
#include <Debug.h>
#include <AppMan.h>
#include <Instrumentor.h>
#include <AssetBrowser.h>
#include <SceneMan.h>

namespace fl
{
	namespace Editor
	{
        namespace MenuBar
        {
			bool dragging = false;
			sf::Vector2i windowDragOffset;
			AssetBrowser browser {"common"};

			void UpdateDrag()
			{
				if (dragging)
				{
					if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) dragging = false;
					else
					{
						AppMan::getWindow()->setPosition(sf::Mouse::getPosition() + windowDragOffset);
					}
				}
				else
				{
					if (ImGui::IsWindowHovered())
					{
						if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
						{
							windowDragOffset = AppMan::getWindow()->getPosition() - sf::Mouse::getPosition();
							dragging = true;
						}
					}
				}
			}

			void DrawBar()
			{
				PROFILE_SCOPE("Inspector");

				// NOTE: (SHORTCUTS ARE CURRENTLY FOR DISPLAY ONLY)
				if (ImGui::BeginMainMenuBar())
				{
					if (ImGui::BeginMenu("File"))
					{
						if (ImGui::MenuItem("Load Scene", "CTRL+O"))
						{
							browser.Launch
							(
								AssetBrowserCallback
								(
									[](std::filesystem::directory_entry entry, std::any data)
									{
										SceneMan::LoadScene(entry.path());
									}, NULL
								),
								"scenes"
							);
						}

						if (ImGui::MenuItem("Save Scene", "CTRL+S"))
						{
							SceneMan::SaveScene("test_scene_save_2");
						}

						ImGui::MenuItem("Import Asset", "CTRL+I");

						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Edit"))
					{
						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Asset"))
					{
						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Entity"))
					{
						ImGui::EndMenu();
					}

					UpdateDrag();
					ImGui::EndMainMenuBar();

					browser.Draw();
				}
			}
        }
	}
}