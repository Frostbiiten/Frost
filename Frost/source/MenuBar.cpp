#include <imgui.h>
#include <Debug.h>
#include <AppMan.h>

namespace fl
{
	namespace Editor
	{
        namespace MenuBar
        {
			bool dragging = false;
			sf::Vector2i windowDragOffset;

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
				// NOTE: (SHORTCUTS ARE CURRENTLY FOR DISPLAY ONLY)

				if (ImGui::BeginMainMenuBar())
				{
					if (ImGui::BeginMenu("File"))
					{
						ImGui::MenuItem("Open Scene", "CTRL+O");
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

					// Close button
					ImGui::SameLine(ImGui::GetWindowWidth() - 30);
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9, 0.0, 0.3, 1));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 0.6, 0.6, 1));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9, 0.4, 0.4, 1));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
					if (ImGui::Button("", ImVec2(25, 25))) AppMan::windowPtr->close();
					ImGui::PopStyleColor(3);
					ImGui::PopStyleVar();

					ImGui::EndMainMenuBar();
				}
			}
        }
	}
}