#include <SceneHierarchy.h>
#include <imgui.h>
#include <fmt/format.h>
#include <Debug.h>

namespace fl
{
	namespace Editor
	{
		// Recursive
		void SceneHierarchy::DrawEntity(entt::entity entity, std::size_t depth = 0)
		{
			// Open tree view
			ImGuiTreeNodeFlags treeFlags = ((selectedEntity != entt::null) && entity == selectedEntity) * ImGuiTreeNodeFlags_Selected;
			treeFlags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;

			bool treeOpen = ImGui::TreeNodeEx(fmt::format("Entity {}", (std::size_t)entity).c_str(), treeFlags);

			// Drag from
			if (ImGui::BeginDragDropSource())
			{
				if (ImGui::SetDragDropPayload("DRAG_DROP_ENTITY", &entity, sizeof(entt::entity)));
				ImGui::Text("Drag and drop");
				ImGui::EndDragDropSource();
			}

			// Drag to
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_DROP_ENTITY"))
				{
					SetParent(sceneRegistry, *(entt::entity*)payload->Data, entity);
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGui::IsItemClicked())
			{
				selectedEntity = entity;
			}

			if (treeOpen)
			{
				Relationship* relationship = sceneRegistry.try_get<Relationship>(entity);
				if (relationship)
				{
					entt::entity currentChild = relationship->children ? relationship->firstChild : entt::null;
					for (std::size_t x = 0; x < relationship->children; ++x)
					{
						DrawEntity(currentChild, depth + 1);

						Relationship* childRelation = sceneRegistry.try_get<Relationship>(currentChild);
						if (childRelation && childRelation->next != entt::null)
						{
							currentChild = childRelation->next;
						}
					}
				}

				ImGui::TreePop();
			}
		}

		void SceneHierarchy::Draw()
		{
			ImGui::Begin("Hierarchy", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			ImGui::SetWindowSize(ImVec2(225, 400));
			ImGui::SetWindowPos(ImVec2(10, 30));

			if (ImGui::IsWindowFocused()) focusedInstance = this;

			sceneRegistry.each([&](auto entity) {
				if (!sceneRegistry.any_of<Parent>(entity))
				{
					DrawEntity(entity);
				}
			});


			ImGui::End();
		}
	}
}