#include "AssetManagerWindow.h"

#include "Grapple/Core/Assert.h"
#include "Grapple/Core/Event.h"

#include "Grapple/Project/Project.h"

#include "GrappleEditor/EditorContext.h"
#include "GrappleEditor/UI/EditorGUI.h"

#include <imgui.h>

namespace Grapple
{
	void AssetManagerWindow::OnImGuiRender()
	{
		ImGui::Begin("Asset Manager");

		if (ImGui::Button("Refresh"))
			RebuildAssetTree();

		ImGui::SameLine();

		ImGuiStyle& style = ImGui::GetStyle();
		
		if (EditorGUI::BeginToggleGroup("Mode", 2))
		{
			if (EditorGUI::ToggleGroupItem("All", m_Mode == AssetTreeViewMode::All))
				m_Mode = AssetTreeViewMode::All;
			if (EditorGUI::ToggleGroupItem("Registry", m_Mode == AssetTreeViewMode::Registry))
				m_Mode = AssetTreeViewMode::Registry;

			EditorGUI::EndToggleGroup();
		}
		
		ImGui::Separator();

		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 14.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));

		if (ImGui::BeginChild("Tree"))
		{
			m_NodeRenderIndex = 0;
			if (m_AssetTree.size())
				RenderDirectory();
			ImGui::EndChild();
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void AssetManagerWindow::RebuildAssetTree()
	{
		if (m_AssetManager == nullptr)
			m_AssetManager = As<EditorAssetManager>(AssetManager::GetInstance());

		m_AssetTree.clear();

		uint32_t rootIndex = 0;
		m_AssetTree.emplace_back("Assets", m_AssetManager->GetRoot());

		BuildDirectory(rootIndex, m_AssetManager->GetRoot());
	}

	void AssetManagerWindow::SetOpenAction(AssetType assetType, const std::function<void(AssetHandle)>& action)
	{
		m_FileOpenActions[assetType] = action;
	}

	void AssetManagerWindow::RenderDirectory()
	{
		const AssetTreeNode& node = m_AssetTree[m_NodeRenderIndex];

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth;
		bool opened = ImGui::TreeNodeEx(node.Name.c_str(), flags, "%s", node.Name.c_str());

		if (opened)
		{
			for (uint32_t i = 0; i < node.ChildrenCount; i++)
			{
				m_NodeRenderIndex++;

				if (m_NodeRenderIndex >= m_AssetTree.size())
					break;

				if (m_AssetTree[m_NodeRenderIndex].IsDirectory)
					RenderDirectory();
				else
					RenderFile();
			}

			ImGui::TreePop();
		}

		m_NodeRenderIndex = node.LastChildIndex;
	}

	void AssetManagerWindow::RenderFile()
	{
		AssetTreeNode& node = m_AssetTree[m_NodeRenderIndex];

		if (m_Mode == AssetTreeViewMode::Registry && !node.IsImported)
			return;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanFullWidth;

		if (!node.IsImported)
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));

		bool opened = ImGui::TreeNodeEx(node.Name.c_str(), flags, "%s", node.Name.c_str());

		if (!node.IsImported)
			ImGui::PopStyleColor();

		if (opened)
		{
			if (ImGui::BeginPopupContextItem(node.Name.c_str()))
			{
				if (node.IsImported && ImGui::MenuItem("Open"))
					OnOpenFile(node);

				if (node.IsImported && ImGui::MenuItem("Remove"))
				{
					Grapple_CORE_ASSERT(node.Handle != NULL_ASSET_HANDLE);
					m_AssetManager->RemoveFromRegistry(node.Handle);
					node.IsImported = false;
				}

				if (!node.IsImported && ImGui::MenuItem("Import"))
				{
					node.Handle = m_AssetManager->ImportAsset(node.Path);
					node.IsImported = node.Handle != NULL_ASSET_HANDLE;
				}

				ImGui::EndMenu();
			}

			ImGui::TreePop();
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			OnOpenFile(node);

		if (node.IsImported && ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload(ASSET_PAYLOAD_NAME, &node.Handle, sizeof(AssetHandle));
			ImGui::EndDragDropSource();
		}
	}

	void AssetManagerWindow::BuildDirectory(uint32_t parentIndex, const std::filesystem::path& path)
	{
		static uint32_t index = 1;
		Grapple_CORE_ASSERT(std::filesystem::is_directory(path));

		for (std::filesystem::path child : std::filesystem::directory_iterator(path))
		{
			if (std::filesystem::is_directory(child))
			{
				m_AssetTree.emplace_back(child.filename().generic_string(), child);
				BuildDirectory((uint32_t) (m_AssetTree.size() - 1), child);
			}
			else
			{
				std::optional<AssetHandle> handle = m_AssetManager->FindAssetByPath(child);

				AssetTreeNode& node = m_AssetTree.emplace_back(child.filename().generic_string(), child, handle.value_or(AssetHandle()));
				node.IsImported = handle.has_value();
			}

			m_AssetTree[parentIndex].LastChildIndex = (uint32_t)(m_AssetTree.size() - 1);
			m_AssetTree[parentIndex].ChildrenCount++;
		}
	}

	void AssetManagerWindow::OnOpenFile(const AssetTreeNode& node)
	{
		Grapple_CORE_ASSERT(!node.IsDirectory);
		Grapple_CORE_ASSERT(node.Handle != NULL_ASSET_HANDLE);

		if (AssetManager::IsAssetHandleValid(node.Handle))
		{
			const AssetMetadata* metadata = AssetManager::GetAssetMetadata(node.Handle);
			if (metadata)
			{
				auto action = m_FileOpenActions.find(metadata->Type);
				if (action != m_FileOpenActions.end())
					action->second(metadata->Handle);
			}
		}
	}
}
