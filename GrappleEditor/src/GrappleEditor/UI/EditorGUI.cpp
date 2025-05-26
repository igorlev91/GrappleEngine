#include "EditorGUI.h"

#include "GrappleCore/Serialization/SerializationStream.h"

#include "Grapple/AssetManager/AssetManager.h"
#include "Grapple/Scene/Scene.h"

#include "GrappleEditor/EditorLayer.h"
#include "GrappleEditor/AssetManager/EditorAssetManager.h"
#include "GrappleEditor/UI/QuickSearch/QuickSearch.h"
#include "GrappleEditor/UI/SerializablePropertyRenderer.h"
#include "GrappleEditor/UI/AssetFieldRenderer.h"

#include <spdlog/spdlog.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace Grapple
{
    static EditorIcons s_EditorIcons(48);

    void EditorGUI::Initialize()
    {
        s_EditorIcons.Initialize();
    }

    bool EditorGUI::BeginPropertyGrid(float width)
    {
        float windowWidth = width == 0.0f ? ImGui::GetContentRegionAvail().x : width;
        if (ImGui::BeginTable("Property Grid", 2))
        {
            const float nameColumnWidthPercetage = 0.3f;

            ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, windowWidth * nameColumnWidthPercetage);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, windowWidth * (1.0f - nameColumnWidthPercetage));
            return true;
        }
        return false;
    }

    void EditorGUI::EndPropertyGrid()
    {
        ImGui::EndTable();
    }

    void EditorGUI::DrawIcon(glm::ivec2 iconPosition, float size)
    {
        if (size == 0.0f)
        {
            const float defaultIconSize = 32.0f;
            size = defaultIconSize;
        }

        ImRect uvs = s_EditorIcons.GetIconUVs(iconPosition);
        ImGui::Image(s_EditorIcons.GetTexture()->GetRendererId(), ImVec2(size, size), uvs.Min, uvs.Max);
    }

    const EditorIcons& EditorGUI::GetIcons()
    {
        return s_EditorIcons;
    }

    void EditorGUI::MoveCursor(ImVec2 offset)
    {
        ImGui::SetCursorPos(ImGui::GetCursorPos() + offset);
    }

    bool EditorGUI::BeginMenu(const char* name)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

        if (ImGui::Button(name))
        {
            ImVec2 buttonMin = ImGui::GetItemRectMin();
            ImVec2 buttonMax = ImGui::GetItemRectMax();
            ImGui::SetNextWindowPos(ImVec2(buttonMin.x, buttonMax.y));
            ImGui::OpenPopup(name);
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(1);
        
        return ImGui::BeginPopup(name, ImGuiWindowFlags_NoMove);
    }

    void EditorGUI::EndMenu()
    {
        ImGui::EndMenu();
    }

    bool EditorGUI::BoolPropertyField(const char* name, bool& value)
    {
        PropertyName(name);

        ImGui::PushID(&value);
        bool result = ImGui::Checkbox("", &value);
        ImGui::PopID();

        ImGui::PopItemWidth();
        return result;
    }

    bool EditorGUI::IntPropertyField(const char* name, int32_t& value)
    {
        PropertyName(name);

        ImGui::PushID(&value);
        bool result = ImGui::DragInt("", &value, 0.1f);
        ImGui::PopID();

        ImGui::PopItemWidth();
        return result;
    }

    bool EditorGUI::UIntPropertyField(const char* name, uint32_t& value)
    {
        PropertyName(name);

        ImGui::PushID(&value);
        bool result = ImGui::DragScalar("", ImGuiDataType_U32, &value);
        ImGui::PopID();

        ImGui::PopItemWidth();
        return result;
    }

    bool EditorGUI::IntVector2PropertyField(const char* name, glm::ivec2& value)
    {
        PropertyName(name);

        ImGui::PushID(&value);
        bool result = ImGui::DragInt2("", glm::value_ptr(value));
        ImGui::PopID();

        ImGui::PopItemWidth();
        return result;
    }

    bool EditorGUI::FloatPropertyField(const char* name, float& value)
    {
        PropertyName(name);

        ImGui::PushID(name);
        bool result = ImGui::DragFloat("", &value, 0.1f);
        ImGui::PopID();

        ImGui::PopItemWidth();
        return result;
    }

    bool EditorGUI::Vector2PropertyField(const char* name, glm::vec2& value)
    {
        PropertyName(name);

        ImGui::PushID(name);
        bool result = ImGui::DragFloat2("", glm::value_ptr(value), 0.1f);
        ImGui::PopID();
        return result;
    }

    bool EditorGUI::Vector3PropertyField(const char* name, glm::vec3& value)
    {
        PropertyName(name);

        ImGui::PushID(name);
        bool result = ImGui::DragFloat3("", glm::value_ptr(value), 0.1f);
        ImGui::PopID();
        return result;
    }

    bool EditorGUI::Vector4PropertyField(const char* name, glm::vec4& value)
    {
        PropertyName(name);

        ImGui::PushID(name);
        bool result = ImGui::DragFloat4("", glm::value_ptr(value), 0.1f);
        ImGui::PopID();
        return result;
    }

    bool EditorGUI::ObjectField(const SerializableObjectDescriptor& descriptor, void* data, const World* currentWorld)
    {
        bool result = false;

        Grapple_CORE_ASSERT(descriptor.Callback);

        SerializablePropertyRenderer propertyRenderer(currentWorld);
        propertyRenderer.PropertyKey(descriptor.Name);
        propertyRenderer.SerializeObject(descriptor, data, false, 0);

        // TODO: get the result from properties renderer
        return result;
    }

    bool EditorGUI::ColorPropertyField(const char* name, glm::vec4& color)
    {
        PropertyName(name);

        ImGui::PushID(&color);
        bool result = ImGui::ColorEdit4("", glm::value_ptr(color), ImGuiColorEditFlags_Float);
        ImGui::PopID();
        return result;
    }

    bool EditorGUI::ColorPropertyField(const char* name, glm::vec3& color)
    {
        PropertyName(name);

        ImGui::PushID(&color);
        bool result = ImGui::ColorEdit3("", glm::value_ptr(color), ImGuiColorEditFlags_Float);
        ImGui::PopID();
        return result;
    }

    static int32_t InputTextCallback(ImGuiInputTextCallbackData* data)
    {
        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
        {
            std::string* str = (std::string*)data->UserData;
            Grapple_CORE_ASSERT(data->Buf == str->c_str());

            str->resize(data->BufTextLen);
            data->Buf = (char*)str->c_str();

            return 1;
        }

        return 0;
    }

    bool EditorGUI::TextProperty(const char* name, std::string& text)
    {
        PropertyName(name);
        return TextField(name, text);
    }

    bool EditorGUI::TextField(const char* name, std::string& text)
    {
        float y = ImGui::GetCursorPosY();
        ImGui::SetCursorPosY(y + ImGui::GetStyle().FramePadding.y);

        ImGui::Text(name);
        ImGui::SameLine();

        ImGui::SetCursorPosY(y);

        return TextField((uint64_t)name, text);
    }

    bool EditorGUI::TextField(std::string& text)
    {
        ImGui::PushID(&text);
        bool result = ImGui::InputTextMultiline("", text.data(), text.size(), ImVec2(0.0f, 0.0f), ImGuiInputTextFlags_CallbackResize, InputTextCallback, (void*)&text);
        ImGui::PopID();
        return result;
    }

    bool EditorGUI::TextField(UUID id, std::string& text)
    {
        ImGui::PushID((int32_t)(uint64_t)id);
        bool result = ImGui::InputText("",
            text.data(),
            text.size(),
            ImGuiInputTextFlags_CallbackResize,
            InputTextCallback,
            (void*)&text);

        ImGui::PopID();
        return result;
    }

    bool EditorGUI::EntityField(const char* name, const World& world, Entity& entity)
    {
        PropertyName(name);
        return EntityField(world, entity);
    }

    bool EditorGUI::EntityField(const World& world, Entity& entity)
    {
        bool alive = world.IsEntityAlive(entity);
        float buttonWidth = ImGui::GetContentRegionAvail().x - 60.0f;

        if (alive)
            ImGui::Button(fmt::format("Enitty {0} {1}", entity.GetIndex(), entity.GetGeneration()).c_str(), ImVec2(buttonWidth, 0));
        else
            ImGui::Button("None", ImVec2(buttonWidth, 0));

        bool result = false;
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ENTITY_PAYLOAD_NAME))
            {
                entity = *(Entity*)payload->Data;
                result = true;
            }

            ImGui::EndDragDropTarget();
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
        {
            entity = Entity();
            return true;
        }

        return result;
    }

    bool EditorGUI::BeginToggleGroup(const char* name, uint32_t itemsCount)
    {
        float itemWidth = ImGui::GetContentRegionAvail().x - (itemsCount) * ImGui::GetStyle().ItemInnerSpacing.x;
        itemWidth /= itemsCount;

        ImGui::PushItemWidth(itemWidth);
        ImGui::GetCurrentWindow()->DC.LayoutType = ImGuiLayoutType_Horizontal;
        return true;
    }

    bool EditorGUI::BeginToggleGroupProperty(const char* name, uint32_t itemsCount)
    {
        PropertyName(name);
        return BeginToggleGroup(name, itemsCount);
    }

    bool EditorGUI::ToggleGroupItem(const char* text, bool selected)
    {
        ImGuiStyle& style = ImGui::GetStyle();

        if (selected)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_TabActive]);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style.Colors[ImGuiCol_TabHovered]);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, style.Colors[ImGuiCol_TabActive]);
        }

        bool result = ImGui::Button(text, ImVec2(ImGui::CalcItemWidth(), 0));

        if (selected)
            ImGui::PopStyleColor(3);

        return result;
    }

    void EditorGUI::EndToggleGroup()
    {
        ImGui::GetCurrentWindow()->DC.LayoutType = ImGuiLayoutType_Vertical;
        ImGui::PopItemWidth();
    }

    void EditorGUI::PropertyName(const char* name, float minHeight)
    {
        ImGui::TableNextRow(0, minHeight);
        ImGui::TableSetColumnIndex(0);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y);
        ImGui::Text(name);

        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-1);
    }

    void EditorGUI::PropertyIndex(size_t index)
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y);
        ImGui::Text("%d", index);

        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-1);
    }

    void EditorGUI::UUIDField(const char* name, UUID uuid)
    {
        PropertyName(name);

        float y = ImGui::GetCursorPosY();

        ImGui::SetCursorPosY(y + ImGui::GetStyle().FramePadding.y);

        ImGui::Text("%llu", (uint64_t)uuid);
    }

    inline static float CalculateAssetPreviewSize()
    {
        const ImGuiStyle& style = ImGui::GetStyle();

        float buttonHeight = style.FramePadding.y * 2.0f + ImGui::GetFontSize();
        return buttonHeight * 2.0f + style.ItemSpacing.y;
    }

    bool EditorGUI::AssetField(AssetHandle& handle, const AssetDescriptor* assetType)
    {
        AssetFieldRenderer fieldRenderer((uint64_t)&handle, handle);
        return fieldRenderer.OnRenderImGui();
    }

    bool EditorGUI::AssetField(Ref<Asset>& asset, const AssetDescriptor& assetDescriptor)
    {
        AssetFieldRenderer fieldRenderer((uint64_t)&asset, asset, assetDescriptor);
        return fieldRenderer.OnRenderImGui();
    }

    bool EditorGUI::AssetField(const char* name, Ref<Asset>& asset, const AssetDescriptor& assetDescriptor)
    {
        PropertyName(name, CalculateAssetPreviewSize());
        AssetFieldRenderer fieldRenderer((uint64_t)&asset, asset, assetDescriptor);
        return fieldRenderer.OnRenderImGui();
    }

    bool EditorGUI::AssetField(const char* name, AssetHandle& handle, const AssetDescriptor* assetType)
    {
        PropertyName(name, CalculateAssetPreviewSize());
        return AssetField(handle, assetType);
    }
}