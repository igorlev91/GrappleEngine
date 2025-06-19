#include "PostProcessingWindow.h"

#include "Grapple/Scene/Scene.h"

#include "GrappleEditor/UI/EditorGUI.h"
#include "GrappleEditor/UI/SerializablePropertyRenderer.h"

#include <imgui.h>

namespace Grapple
{
	PostProcessingWindow::PostProcessingWindow(Ref<Scene> scene)
		: m_Scene(scene)
	{
	}

	void PostProcessingWindow::OnImGuiRender()
	{
		if (m_Scene == nullptr)
			return;

		if (ImGui::Begin("Post Processing"))
		{
			RenderWindowContents();
		}

		ImGui::End();
	}

	void PostProcessingWindow::RenderWindowContents()
	{
		Grapple_CORE_ASSERT(m_Scene);
		PostProcessingManager& postProcessingManager = m_Scene->GetPostProcessingManager();

		auto& postProcessing = m_Scene->GetPostProcessingManager();
		float windowWidth = ImGui::GetContentRegionAvail().x;

		ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_FramePadding
			| ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_AllowItemOverlap;

		for (const auto& entry : postProcessing.GetEntries())
		{
			ImVec2 initialCursorPosition = ImGui::GetCursorPos();
			ImGui::SetNextItemWidth(windowWidth - 60.0f);

			const char* effectName = entry.Descriptor->Name.c_str();

			ImGui::PushID(effectName);
			bool expanded = ImGui::TreeNodeEx("", treeNodeFlags);
			ImGui::PopID();

			ImVec2 cursorPosition = ImGui::GetCursorPos();
			ImVec2 itemSize = ImGui::GetItemRectSize();

			bool enabled = entry.Effect->IsEnabled();
			if (RenderCheckBoxAndLabel(entry.Effect.get(), initialCursorPosition, itemSize, &enabled, effectName))
				entry.Effect->SetEnabled(enabled);

			ImGui::SetCursorPos(cursorPosition);

			if (expanded)
			{
				SerializablePropertyRenderer propertyRenderer(&m_Scene->GetECSWorld());
				entry.Descriptor->Callback(entry.Effect.get(), propertyRenderer);

				if (propertyRenderer.PropertiesGridStarted())
				{
					EditorGUI::EndPropertyGrid();
				}

				ImGui::TreePop();
			}
		}
	}

	bool PostProcessingWindow::RenderCheckBoxAndLabel(void* id, ImVec2 position, ImVec2 treeNodeSize, bool* enabled, const char* label)
	{
		float checkBoxSize = ImGui::GetFrameHeight();
		float checkBoxOffset = 30.0f;

		ImGui::SetCursorPos(ImVec2(position.x + checkBoxOffset, position.y + treeNodeSize.y / 2.0f - checkBoxSize / 2.0f));

		ImGui::PushID(id);
		bool result = ImGui::Checkbox("", enabled);
		ImGui::PopID();

		ImVec2 textSize = ImGui::CalcTextSize(label);

		const auto& style = ImGui::GetStyle();

		float labelOffset = position.x + checkBoxOffset + checkBoxSize + style.ItemSpacing.x;
		ImGui::SetCursorPos(ImVec2(labelOffset, position.y + treeNodeSize.y / 2.0f - textSize.y / 2.0f));
		ImGui::TextUnformatted(label);

		return result;
	}
}
