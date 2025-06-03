#include "ShaderLibraryWindow.h"

#include "GrappleCore/Profiler/Profiler.h"

#include "Grapple/Core/Application.h"

#include "Grapple/AssetManager/AssetManager.h"
#include "Grapple/Renderer/ShaderLibrary.h"

#include "GrappleEditor/ImGui/ImGuiLayer.h"
#include "GrappleEditor/UI/EditorGUI.h"
#include "GrappleEditor/AssetManager/EditorAssetManager.h"

namespace Grapple
{
	static ShaderLibraryWindow s_Instance;

	void ShaderLibraryWindow::OnRenderImGui()
	{
		Grapple_PROFILE_FUNCTION();
		if (m_Show && ImGui::Begin("Shader Library", &m_Show))
		{
			const ImGuiStyle& style = ImGui::GetStyle();
			for (const auto& [name, handle] : ShaderLibrary::GetNameToHandleMap())
			{
				ImGui::Separator();
				if (EditorGUI::BeginPropertyGrid())
				{
					RenderShaderItem(handle);
					EditorGUI::EndPropertyGrid();
				}
			}

			ImGui::End();
		}
	}

	void ShaderLibraryWindow::Show()
	{
		s_Instance.m_Show = true;
	}

	ShaderLibraryWindow& ShaderLibraryWindow::GetInstance()
	{
		return s_Instance;
	}

	void ShaderLibraryWindow::RenderShaderItem(AssetHandle handle)
	{
		Grapple_CORE_ASSERT(AssetManager::IsAssetHandleValid(handle));
		const AssetMetadata* metadata = AssetManager::GetAssetMetadata(handle);

		const ImGuiStyle& style = ImGui::GetStyle();

		ImGui::Separator();
		EditorGUI::PropertyName("Name");
		EditorGUI::MoveCursor(ImVec2(0.0f, style.FramePadding.y));
		ImGui::Text("%s", metadata->Name.c_str());
		EditorGUI::MoveCursor(ImVec2(0.0f, -style.FramePadding.y));

		EditorGUI::PropertyName("Handle");
		EditorGUI::MoveCursor(ImVec2(0.0f, style.FramePadding.y));
		ImGui::Text("%llu", (uint64_t)handle);
		EditorGUI::MoveCursor(ImVec2(0.0f, style.FramePadding.y));

		EditorGUI::PropertyName("Path");
		EditorGUI::MoveCursor(ImVec2(0.0f, style.FramePadding.y));
		ImGui::Text("%s", metadata->Path.generic_string().c_str());
		EditorGUI::MoveCursor(ImVec2(0.0f, style.FramePadding.y));
	}
}
