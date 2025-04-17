#include "SceneWindow.h"

#include "Grapple/Scene/Components.h"
#include "Grapple/Scene/Scene.h"

#include "GrappleECS/World.h"
#include "GrappleECS/Query/EntitiesIterator.h"
#include "GrappleECS/Entities.h"

#include "GrappleEditor/ImGui/ImGuiLayer.h"
#include "GrappleEditor/EditorLayer.h"
#include "GrappleEditor/UI/EditorGUI.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Grapple
{
	void SceneWindow::OnImGuiRender()
	{
		ImGui::Begin("Scene");

		if (Scene::GetActive() == nullptr)
		{
			ImGui::End();
			return;
		}

		World& world = Scene::GetActive()->GetECSWorld();
		const auto& records = world.Entities.GetEntityRecords();

		m_Hierarchy.SetWorld(world);

		Entity selected;
		if (EditorLayer::GetInstance().Selection.GetType() == EditorSelectionType::Entity)
			selected = EditorLayer::GetInstance().Selection.GetEntity();

		if (m_Hierarchy.OnRenderImGui(selected))
			EditorLayer::GetInstance().Selection.SetEntity(selected);

		ImGui::End();
	}
}