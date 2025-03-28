#include "SceneWindow.h"

#include "Grapple/Scene/Components.h"

#include "GrappleECS/World.h"
#include "GrappleECS/Query/EntityRegistryIterator.h"
#include "GrappleECS/Registry.h"

#include "GrappleEditor/EditorContext.h"

#include <imgui.h>

namespace Grapple
{
	void SceneWindow::OnImGuiRender()
	{
		World& world = EditorContext::GetActiveScene()->GetECSWorld();

		ImGui::Begin("Scene");

		if (ImGui::BeginPopupContextWindow("Scene Context Menu"))
		{
			if (ImGui::MenuItem("Create Entity"))
				world.CreateEntity<TransformComponent>();

			ImGui::EndMenu();
		}

		std::optional<Entity> deletedEntity;
		for (Entity entity : world.GetRegistry())
		{
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

		if (deletedEntity.has_value())
			world.DeleteEntity(deletedEntity.value());

		ImGui::End();
	}
}