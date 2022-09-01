#include <Inspector.h>
#include <imgui.h>
#include <AppMan.h>
#include <SceneHierarchy.h>
#include <queue>
#include <variant>

#include <SpriteRenderer.h>
#include <imgui-SFML.h>

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
			ImGui::Begin("Inspector", NULL);

			if (focusedInstance && focusedInstance->selectedEntity != entt::null)
			{
				// Title
				entt::entity focusedEntity = focusedInstance->selectedEntity;
				ImGui::Text(fmt::format("Entity {}", (std::size_t)focusedEntity).c_str());
				ImGui::Separator();

				// Parent

				if (ImGui::CollapsingHeader("Parent"))
				{
					if (sceneRegistry.any_of<Parent>(focusedEntity))
					{
						Parent& parentComponent = sceneRegistry.get<Parent>(focusedEntity);

						if (parentComponent.entity != entt::null)
						{
							bool parentSelect = ImGui::Button(fmt::format("Parent Entity: {}", (std::size_t)parentComponent.entity).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 25));

							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Click to select parent");
								ImGui::Text("Right click to unlink parent");
								ImGui::EndTooltip();

								if (ImGui::IsMouseDown(ImGuiMouseButton_Left) || parentSelect)
								{
									focusedInstance->selectedEntity = parentComponent.entity;
								}
								else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
								{
									SetParent(sceneRegistry, focusedEntity, entt::null);
								}
							}
						}
						else
						{
							ImGui::SameLine();
							ImGui::TextDisabled("NULL Parent");
						}
					}
					else
					{
						ImGui::TextDisabled("No Parent Component");
					}

					ImGui::Separator();
				}

				// Transform
				if (sceneRegistry.any_of<Transform>(focusedEntity))
				{
					bool visible = true;
					if (ImGui::CollapsingHeader("Transform", &visible))
					{
						Transform& transform = sceneRegistry.get<Transform>(focusedEntity);

						InputVec2(transform.position, "Position");
						InputVec2(transform.pivot, "Pivot");
						InputVec2(transform.scale, "Scale");
						InputFloat(transform.rotation, "Rotation");
						InputFloat(transform.depth, "Depth");

						ImGui::Separator();
					}

					// Delete
					if (!visible) sceneRegistry.remove<Transform>(focusedEntity);
				}

				// SpriteRenderer
				if (sceneRegistry.any_of<SpriteRenderer>(focusedEntity))
				{
					bool visible = true;
					if (ImGui::CollapsingHeader("SpriteRenderer", &visible))
					{
						SpriteRenderer& spriteRenderer = sceneRegistry.get<SpriteRenderer>(focusedEntity);

						if (ImGui::BeginTable("Separator", 2, ImGuiTableFlags_Resizable))
						{
							ImGui::TableNextColumn();
							ImGui::Text("Texture");

							if (spriteRenderer.texture)
							{
								ImGui::TableNextColumn();
								bool textureSelect = ImGui::ImageButton(*spriteRenderer.texture, sf::Vector2f(ImGui::GetContentRegionAvail().x - 20, ImGui::GetContentRegionAvail().x - 20));

								if (ImGui::IsItemHovered())
								{
									ImGui::BeginTooltip();
									ImGui::Text("Click to select texture");
									ImGui::Text("Right click to unlink texture");
									ImGui::EndTooltip();

									if (ImGui::IsMouseDown(ImGuiMouseButton_Left) || textureSelect)
									{
										Debug::log()->error("Not implemented yet!!!");
									}
									else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
									{
										spriteRenderer.texture = NULL;
									}
								}
							}
							else
							{
								ImGui::BeginDisabled();
								ImGui::TextWrapped("No texture selected");
								ImGui::EndDisabled();
								ImGui::TableNextColumn();
								bool textureSelect = ImGui::ImageButton(*ResourceMan::getTexture("common/null.png"), sf::Vector2f(ImGui::GetContentRegionAvail().x - 20, ImGui::GetContentRegionAvail().x - 20));
							}

							ImGui::EndTable();
							ImGui::Separator();
						}
					}

					// Delete
					if (!visible) sceneRegistry.remove<SpriteRenderer>(focusedEntity);
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
