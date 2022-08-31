#include <Inspector.h>
#include <imgui.h>
#include <AppMan.h>
#include <SceneHierarchy.h>
#include <queue>
#include <variant>

namespace fl
{
	namespace Editor
	{
		// This may be able to be utilized for undo/redo?
		// Perhaps this could be replaced with a queue of functions (lambas passed in?)
		std::queue<std::variant<
			EditAction<sf::Vector2f>,
			EditAction<std::string>,
			EditAction<float>,
			EditAction<int>
		>> editQueue;

		void InputVec2(sf::Vector2f& vec, std::string_view name)
		{
			float input[2];
			input[0] = vec.x;
			input[1] = vec.y;
			if (ImGui::InputFloat2(name.data(), input, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue) && (vec.x != input[0] || vec.y != input[1]))
			{
				editQueue.push(EditAction<sf::Vector2f>(vec, sf::Vector2f { input[0], input[1] }));
			}
		}

		void InputFloat(float& vec, std::string_view name)
		{
			float input = vec;
			if (ImGui::InputFloat(name.data(), &input, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue) && (vec != input))
			{
				editQueue.push(EditAction<float>(vec, input));
			}
		}

		void Inspector::Draw()
		{
			ImGui::Begin("Inspector", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			ImGui::SetWindowSize(ImVec2(225, AppMan::windowPtr->getSize().y - 40));
			ImGui::SetWindowPos(ImVec2(AppMan::windowPtr->getSize().x - 235, 30));

			if (focusedInstance && focusedInstance->selectedEntity != entt::null)
			{
				entt::entity focusedEntity = focusedInstance->selectedEntity;
				ImGui::Text(fmt::format("Entity {}", (std::size_t)focusedEntity).c_str());

				ImGui::SameLine();
				if (sceneRegistry.any_of<Parent>(focusedEntity))
				{
					Parent& parentComponent = sceneRegistry.get<Parent>(focusedEntity);

					if (parentComponent.entity != entt::null)
					{
						if (ImGui::Selectable(fmt::format("Parent: {}", (std::size_t)parentComponent.entity).c_str()))
						{
							focusedInstance->selectedEntity = parentComponent.entity;
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Text("Click to select parent");
							ImGui::EndTooltip();
						}
					}
					else
					{
						ImGui::TextDisabled("NULL Parent");
					}
				}
				else
				{
					ImGui::TextDisabled("No Parent Component");
				}
				ImGui::Separator();

				if (sceneRegistry.any_of<Transform>(focusedEntity))
				{
					Transform& transform = sceneRegistry.get<Transform>(focusedEntity);

					InputVec2(transform.position, "Position");
					InputVec2(transform.pivot, "Pivot");
					InputVec2(transform.scale, "Scale");
					InputFloat(transform.rotation, "Rotation");
					InputFloat(transform.depth, "Depth");
				}
			}
			else
			{
				ImGui::TextDisabled("Nothing Selected...\n");
			}

			ImGui::End();
		}

		void Inspector::ProcessEditQueue()
		{
			while (!editQueue.empty())
			{
				auto& edit = editQueue.front();

				// I'm not sure how exactly to use templates but it feels like this could be a lot shorter
				if (std::holds_alternative<EditAction<sf::Vector2f>>(edit)) std::get<EditAction<sf::Vector2f>>(edit).Process();
				else if (std::holds_alternative<EditAction<std::string>>(edit)) std::get<EditAction<std::string>>(edit).Process();
				else if (std::holds_alternative<EditAction<float>>(edit)) std::get<EditAction<float>>(edit).Process();
				else if (std::holds_alternative<EditAction<int>>(edit)) std::get<EditAction<int>>(edit).Process();

				editQueue.pop();
			}
		}
	}
}
