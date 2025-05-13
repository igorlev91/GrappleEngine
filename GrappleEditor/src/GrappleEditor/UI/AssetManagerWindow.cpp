#include "AssetManagerWindow.h"

#include "GrappleCore/Assert.h"

#include "Grapple/Project/Project.h"

#include "Grapple/Renderer/Material.h"
#include "Grapple/Renderer/ShaderLibrary.h"

#include "GrappleEditor/EditorLayer.h"
#include "GrappleEditor/UI/EditorGUI.h"

#include "GrappleEditor/AssetManager/MaterialImporter.h"

#include <fstream>
#include <imgui.h>

namespace Grapple
{
    void AssetManagerWindow::OnImGuiRender()
    {
        ImGui::Begin("Asset Manager");

        if (ImGui::Button("Refresh"))
            RebuildAssetTree();

        ImGuiStyle& style = ImGui::GetStyle();

        const float maxTreeWidth = 300.0f;

        ImVec2 treeSize = ImGui::GetContentRegionAvail();
        treeSize.x = glm::min(maxTreeWidth, treeSize.x);
        
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 14.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));

        if (ImGui::BeginChild("Tree", treeSize))
        {
            m_NodeRenderIndex = 0;
            if (m_AssetTree.size())
                RenderDirectory();
            ImGui::EndChild();
        }

        RenderCreateNewFilePopup();

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
        std::filesystem::path root = AssetRegistrySerializer::GetAssetsRoot();
        m_AssetTree.emplace_back("Assets", root);

        BuildDirectory(rootIndex, root);
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
        if (ImGui::BeginPopupContextItem(node.Name.c_str()))
        {
            if (ImGui::BeginMenu("New"))
            {
                if (ImGui::MenuItem("Prefab"))
                {
                    m_ShowNewFileNamePopup = true;
                    m_OnNewFileNameEntered = [this, nodeIndex = m_NodeRenderIndex](std::string_view name)
                    {
                        Grapple_CORE_ASSERT(m_AssetTree[nodeIndex].IsDirectory);
                        std::filesystem::path path = m_AssetTree[nodeIndex].Path / name;
                        path.replace_extension(".flrprefab");

                        if (!std::filesystem::exists(path))
                        {
                            std::ofstream output(path);
                            output << R"(Components:
  - Name: struct Grapple::TransformComponent
    Position: [0, 0, 0]
    Rotation: [0, 0, 0]
    Scale: [1, 1, 1])";
                        }
                    };
                }

                if (ImGui::MenuItem("Material"))
                {
                    m_ShowNewFileNamePopup = true;
                    m_OnNewFileNameEntered = [this, nodeIndex = m_NodeRenderIndex](std::string_view name)
                    {
                        std::filesystem::path path = m_AssetTree[nodeIndex].Path / name;
                        path.replace_extension(".flrmat");

                        std::optional<AssetHandle> meshShaderHandle = ShaderLibrary::FindShader("Mesh");

                        if (meshShaderHandle)
                        {
                            Ref<Material> material = CreateRef<Material>(meshShaderHandle.value());
                            MaterialImporter::SerializeMaterial(material, path);

                            m_AssetManager->ImportAsset(path, material);
                            RebuildAssetTree();
                        }
                        else
                        {
                            Grapple_CORE_ERROR("Failed to find Mesh shader");
                        }
                    };
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

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

        if (node.ChildrenCount != 0)
            m_NodeRenderIndex = node.LastChildIndex;
    }

    void AssetManagerWindow::RenderFile()
    {
        AssetTreeNode& node = m_AssetTree[m_NodeRenderIndex];

        if (node.IsImported)
        {
            RenderAssetItem(node.Handle);
            return;
        }
        else
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
                | ImGuiTreeNodeFlags_FramePadding
                | ImGuiTreeNodeFlags_SpanFullWidth
                | ImGuiTreeNodeFlags_Leaf;

            const ImGuiStyle& style = ImGui::GetStyle();
            ImGui::PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]);

            bool opened = ImGui::TreeNodeEx(node.Name.c_str(), flags, node.Name.c_str());

            if (opened)
            {
                if (ImGui::BeginPopupContextItem(node.Name.c_str()))
                {
                    if (ImGui::MenuItem("Import"))
                    {
                        node.Handle = m_AssetManager->ImportAsset(node.Path);
                        node.IsImported = node.Handle != NULL_ASSET_HANDLE;
                    }

                    ImGui::EndMenu();
                }

                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
        }
    }

    void AssetManagerWindow::RenderAssetItem(AssetHandle handle)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth;

        Grapple_CORE_ASSERT(AssetManager::IsAssetHandleValid(handle));

        const AssetMetadata* metadata = AssetManager::GetAssetMetadata(handle);
        if (metadata->SubAssets.size() == 0)
            flags |= ImGuiTreeNodeFlags_Leaf;

        bool opened = ImGui::TreeNodeEx(metadata->Name.c_str(), flags, metadata->Name.c_str());

        // Drag and drop

        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload(ASSET_PAYLOAD_NAME, &handle, sizeof(AssetHandle));
            ImGui::EndDragDropSource();
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            OnOpenFile(handle);

        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            EditorLayer::GetInstance().Selection.SetAsset(handle);
            OnAssetSelectionChanged.Invoke(handle);
        }

        // Context menu

        if (ImGui::BeginPopupContextItem(metadata->Name.c_str()))
        {
            if (ImGui::MenuItem("Open"))
                OnOpenFile(handle);

            if (ImGui::MenuItem("Remove"))
            {
                for (AssetTreeNode& node : m_AssetTree)
                {
                    if (node.Handle == handle)
                    {
                        node.IsImported = false;
                        break;
                    }
                }

                m_AssetManager->RemoveFromRegistry(handle);
            }

            if (ImGui::MenuItem("Reload"))
            {
                if (AssetManager::IsAssetLoaded(handle))
                    m_AssetManager->ReloadAsset(handle);
            }

            ImGui::EndMenu();
        }

        if (opened)
        {
            for (AssetHandle handle : metadata->SubAssets)
                RenderAssetItem(handle);

            ImGui::TreePop();
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

    void AssetManagerWindow::OnOpenFile(AssetHandle handle)
    {
        if (AssetManager::IsAssetHandleValid(handle))
        {
            const AssetMetadata* metadata = AssetManager::GetAssetMetadata(handle);
            if (metadata)
            {
                auto action = m_FileOpenActions.find(metadata->Type);
                if (action != m_FileOpenActions.end())
                    action->second(metadata->Handle);
            }
        }
    }

    void AssetManagerWindow::RenderCreateNewFilePopup()
    {
        bool createNewFile = false;

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (m_ShowNewFileNamePopup)
        {
            m_ShowNewFileNamePopup = false;
            std::memset(m_TextInputBuffer, 0, sizeof(m_TextInputBuffer));
            ImGui::OpenPopup("EnterNewFileName");
        }

        if (ImGui::BeginPopupModal("EnterNewFileName", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y);
            ImGui::Text("Name");
            ImGui::SameLine();

            ImGui::PushID("FileNameField");
            ImGui::InputText("", m_TextInputBuffer, 512);
            ImGui::PopID();

            if (ImGui::Button("Create"))
                createNewFile = true;

            if (ImGui::IsKeyPressed(ImGuiKey_Enter))
                createNewFile = true;
            if (ImGui::IsKeyPressed(ImGuiKey_Escape))
                ImGui::CloseCurrentPopup();

            ImGui::SameLine();

            if (ImGui::Button("Cancel"))
                ImGui::CloseCurrentPopup();

            if (createNewFile)
                ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }

        if (createNewFile)
        {
            std::string_view fileName = m_TextInputBuffer;

            if (m_OnNewFileNameEntered)
            {
                m_OnNewFileNameEntered(fileName);
                m_OnNewFileNameEntered = nullptr;
            }
        }
    }
}
