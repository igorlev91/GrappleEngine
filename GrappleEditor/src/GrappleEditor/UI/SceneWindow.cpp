#include "SceneWindow.h"

#include "GrappleECS/World.h"
#include "GrappleECS/Query/EntityRegistryIterator.h"
#include "GrappleECS/Registry.h"

#include "GrappleEditor/EditorContext.h"

#include <imgui.h>

namespace Grapple
{
	void SceneWindow::OnImGuiRender()
	{
		ImGui::Begin("Scene");

		World& world = EditorContext::Instance.ActiveScene->GetECSWorld();
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
		}

		ImGui::End();
	}
}