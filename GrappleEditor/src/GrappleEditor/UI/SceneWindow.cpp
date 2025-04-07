#include "SceneWindow.h"

#include "Grapple/Scene/Components.h"

#include "GrappleECS/World.h"
#include "GrappleECS/Query/EntityRegistryIterator.h"
#include "GrappleECS/Registry.h"

#include "GrappleEditor/EditorContext.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Grapple
{
	void SceneWindow::OnImGuiRender()
	{
		World& world = EditorContext::GetActiveScene()->GetECSWorld();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Scene");

		if (ImGui::BeginPopupContextWindow("Scene Context Menu"))
		{
			if (ImGui::MenuItem("Create Entity"))
				world.CreateEntity<TransformComponent>();

			ImGui::EndMenu();
		}

		const auto& records = world.GetRegistry().GetEntityRecords();

		ImGui::BeginChild("Scene Entities");
		ImGuiListClipper clipper;
		clipper.Begin(records.size());

		std::optional<Entity> deletedEntity;
		while (clipper.Step())
		{
			for (int32_t i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			{
				if (i < 0 || i >= (int32_t)records.size())
					continue;

				Entity entity = records[i].Id;
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;

				bool opened = ImGui::TreeNodeEx((void*)std::hash<Entity>()(entity), flags, "Entity %d", entity.GetIndex());

				if (ImGui::IsItemClicked())
					EditorContext::Instance.SelectedEntity = entity;

				if (opened)
				{
					ImGui::TreePop();
				}

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
		ImGui::PopStyleVar();
	}
}