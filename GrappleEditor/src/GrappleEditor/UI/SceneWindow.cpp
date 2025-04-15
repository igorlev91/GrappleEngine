#include "SceneWindow.h"

#include "Grapple/Scene/Components.h"
#include "Grapple/Scene/Scene.h"

#include "GrappleECS/World.h"
#include "GrappleECS/Query/EntityRegistryIterator.h"
#include "GrappleECS/Registry.h"

#include "GrappleEditor/ImGui/ImGuiLayer.h"
#include "GrappleEditor/EditorLayer.h"
#include "GrappleEditor/UI/EditorGUI.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Grapple
{
	void SceneWindow::OnImGuiRender()
	{
		World& world = Scene::GetActive()->GetECSWorld();

		ImGui::Begin("Scene");

		const auto& records = world.GetRegistry().GetEntityRecords();

		ImGui::BeginChild("Scene Entities");
		ImGuiListClipper clipper;
		clipper.Begin((int32_t)records.size());

		if (ImGui::BeginPopupContextWindow("Scene Context Menu"))
		{
			if (ImGui::MenuItem("Create Entity"))
				world.CreateEntity<TransformComponent>();

			ImGui::EndMenu();
		}

		std::optional<Entity> deletedEntity;
		while (clipper.Step())
		{
			for (int32_t i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			{
				if (i < 0 || i >= (int32_t)records.size())
					continue;

				Entity entity = records[i].Id;
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
					| ImGuiTreeNodeFlags_FramePadding
					| ImGuiTreeNodeFlags_Bullet
					| ImGuiTreeNodeFlags_SpanFullWidth;

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImGui::GetStyle().FramePadding / 2);

				bool selected = entity == EditorLayer::GetInstance().GetSelectedEntity();
				if (selected)
				{
					flags |= ImGuiTreeNodeFlags_Framed;
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
					ImGui::PushStyleColor(ImGuiCol_Header, ImGuiTheme::Primary);
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImGuiTheme::Primary);
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGuiTheme::PrimaryVariant);
				}

				bool opened = ImGui::TreeNodeEx((void*)std::hash<Entity>()(entity), flags, "Entity %d", entity.GetIndex());

				if (selected)
				{
					ImGui::PopStyleVar(1);
					ImGui::PopStyleColor(3);
				}

				ImGui::PopStyleVar(1);

				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload(ENTITY_PAYLOAD_NAME, &entity, sizeof(Entity));
					ImGui::EndDragDropSource();
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
					EditorLayer::GetInstance().SetSelectedEntity(entity);

				if (opened)
					ImGui::TreePop();

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete entity"))
						deletedEntity = entity;

					ImGui::EndMenu();
				}
			}
		}

		clipper.End();
		ImGui::EndChild();

		if (deletedEntity.has_value())
			world.DeleteEntity(deletedEntity.value());

		ImGui::End();
	}
}