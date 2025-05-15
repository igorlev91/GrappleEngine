#include "SerializablePropertyRenderer.h"

#include "Grapple/Renderer/Material.h"
#include "Grapple/Renderer/Texture.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "GrappleEditor/UI/EditorGUI.h"
#include "GrappleEditor/ImGui/ImGuiLayer.h"

namespace Grapple
{
    SerializablePropertyRenderer::SerializablePropertyRenderer(const World* world)
        : m_CurrentState({ false }), m_CurrentWorld(world) {}

    void SerializablePropertyRenderer::PropertyKey(std::string_view key)
    {
        m_CurrentPropertyName = key;
    }

    SerializationStream::DynamicArrayAction SerializablePropertyRenderer::SerializeDynamicArraySize(size_t& size)
    {
        BeginPropertiesGridIfNeeded();
        DynamicArrayAction action = DynamicArrayAction::None;

        EditorGUI::PropertyName("Size");
        ImGui::PushID(&size);
        if (ImGui::InputScalar("", ImGuiDataType_U64, &size))
            action = DynamicArrayAction::Resize;

        ImGui::PopID();
        
        return action;
    }

    void SerializablePropertyRenderer::SerializeInt(SerializationValue<uint8_t> intValues, SerializableIntType type)
    {
        BeginPropertiesGridIfNeeded();

        if (!intValues.IsArray)
            EditorGUI::PropertyName(m_CurrentPropertyName.data());

        ImGuiDataType dataType = (ImGuiDataType)0;
        size_t intSize = SizeOfSerializableIntType(type);

        switch (type)
        {
#define INT_TYPE_TO_IMGUI_TYPE(intType, imGuiType)  \
        case SerializableIntType::intType: \
            dataType = imGuiType;          \
            break;

        INT_TYPE_TO_IMGUI_TYPE(Int8, ImGuiDataType_S8);
        INT_TYPE_TO_IMGUI_TYPE(UInt8, ImGuiDataType_U8);
        INT_TYPE_TO_IMGUI_TYPE(Int16, ImGuiDataType_S16);
        INT_TYPE_TO_IMGUI_TYPE(UInt16, ImGuiDataType_U16);
        INT_TYPE_TO_IMGUI_TYPE(Int32, ImGuiDataType_S32);
        INT_TYPE_TO_IMGUI_TYPE(UInt32, ImGuiDataType_U32);
        INT_TYPE_TO_IMGUI_TYPE(Int64, ImGuiDataType_S64);
        INT_TYPE_TO_IMGUI_TYPE(UInt64, ImGuiDataType_U64);

#undef INT_TYPE_TO_IMGUI_SCALAR_TYPE
        }

        for (size_t i = 0; i < intValues.Values.GetSize(); i += intSize)
        {
            if (intValues.IsArray)
                EditorGUI::PropertyIndex(i / intSize);

            ImGui::PushID(&intValues.Values[i]);
            ImGui::DragScalar("", dataType, &intValues.Values[i]);
            ImGui::PopID();
        }
    }

    void SerializablePropertyRenderer::SerializeBool(SerializationValue<bool> value)
    {
        BeginPropertiesGridIfNeeded();

        if (!value.IsArray)
            EditorGUI::PropertyName(m_CurrentPropertyName.data());

        for (size_t i = 0; i < value.Values.GetSize(); i++)
        {
            if (value.IsArray)
                EditorGUI::PropertyIndex(i);

            ImGui::PushID(&value.Values[i]);
            ImGui::Checkbox("", &value.Values[i]);
            ImGui::PopID();
        }
    }

    void SerializablePropertyRenderer::SerializeFloat(SerializationValue<float> value)
    {
        BeginPropertiesGridIfNeeded();
        
        if (!value.IsArray)
            EditorGUI::PropertyName(m_CurrentPropertyName.data());

        for (size_t i = 0; i < value.Values.GetSize(); i++)
        {
            if (value.IsArray)
                EditorGUI::PropertyIndex(i);

            ImGui::PushID(&value.Values[i]);
            ImGui::DragFloat("", &value.Values[i]);
            ImGui::PopID();
        }
    }

    void SerializablePropertyRenderer::SerializeUUID(SerializationValue<UUID> uuids)
    {
        BeginPropertiesGridIfNeeded();
        
        if (!uuids.IsArray)
            EditorGUI::PropertyName(m_CurrentPropertyName.data());

        for (size_t i = 0; i < uuids.Values.GetSize(); i++)
        {
            if (uuids.IsArray)
                EditorGUI::PropertyIndex(i);

            ImGui::Text("%llu", (uint64_t)uuids.Values[i]);
        }
    }

    void SerializablePropertyRenderer::SerializeFloatVector(SerializationValue<float> value, uint32_t componentsCount)
    {
        BeginPropertiesGridIfNeeded();

        if (!value.IsArray)
            EditorGUI::PropertyName(m_CurrentPropertyName.data());

        for (size_t i = 0; i < value.Values.GetSize(); i += (size_t)componentsCount)
        {
            if (value.IsArray)
                EditorGUI::PropertyIndex(i / (size_t)componentsCount);

            ImGui::PushID(&value.Values[i]);
            switch (componentsCount)
            {
            case 1:
                ImGui::DragFloat("", &value.Values[i]);
                break;
            case 2:
                ImGui::DragFloat2("", &value.Values[i]);
                break;
            case 3:
                if (HAS_BIT(value.Flags, SerializationValueFlags::Color))
                    ImGui::ColorEdit3("", &value.Values[i], ImGuiColorEditFlags_Float);
                else if (HAS_BIT(value.Flags, SerializationValueFlags::Color))
                    ImGui::ColorEdit3("", &value.Values[i], ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
                else
                    ImGui::DragFloat3("", &value.Values[i]);
                break;
            case 4:
                if (HAS_BIT(value.Flags, SerializationValueFlags::Color))
                    ImGui::ColorEdit4("", &value.Values[i], ImGuiColorEditFlags_Float);
                else if (HAS_BIT(value.Flags, SerializationValueFlags::Color))
                    ImGui::ColorEdit4("", &value.Values[i], ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
                else
                    ImGui::DragFloat4("", &value.Values[i]);
                break;
            }

            ImGui::PopID();
        }
    }

    void SerializablePropertyRenderer::SerializeIntVector(SerializationValue<int32_t> value, uint32_t componentsCount)
    {
        BeginPropertiesGridIfNeeded();

        if (!value.IsArray)
            EditorGUI::PropertyName(m_CurrentPropertyName.data());
        
        for (size_t i = 0; i < value.Values.GetSize(); i += (size_t)componentsCount)
        {
            if (value.IsArray)
                EditorGUI::PropertyIndex(i / (size_t)componentsCount);

            ImGui::PushID(&value.Values[i]);
            switch (componentsCount)
            {
            case 1:
                ImGui::DragInt("", &value.Values[i]);
                break;
            case 2:
                ImGui::DragInt2("", &value.Values[i]);
                break;
            case 3:
                ImGui::DragInt3("", &value.Values[i]);
                break;
            case 4:
                ImGui::DragInt4("", &value.Values[i]);
                break;
            }

            ImGui::PopID();
        }
    }

    void SerializablePropertyRenderer::SerializeString(SerializationValue<std::string> value)
    {
        BeginPropertiesGridIfNeeded();

        if (!value.IsArray)
            EditorGUI::PropertyName(m_CurrentPropertyName.data());

        for (size_t i = 0; i < value.Values.GetSize(); i++)
        {
            if (value.IsArray)
                EditorGUI::PropertyIndex(i);
            
            EditorGUI::TextField(value.Values[i]);
        }
    }

    void SerializablePropertyRenderer::SerializeObject(const SerializableObjectDescriptor& descriptor, void* objectData)
    {
        if (&descriptor == &Grapple_SERIALIZATION_DESCRIPTOR_OF(AssetHandle))
        {
            BeginPropertiesGridIfNeeded();
            EditorGUI::PropertyName(m_CurrentPropertyName.data());
            RenderAssetField(*reinterpret_cast<AssetHandle*>(objectData));
            return;
        }

        if (&descriptor == &Grapple_SERIALIZATION_DESCRIPTOR_OF(Entity))
        {
            if (m_CurrentWorld == nullptr)
                return;

            BeginPropertiesGridIfNeeded();
            EditorGUI::EntityField(m_CurrentPropertyName.data(), *m_CurrentWorld, *reinterpret_cast<Entity*>(objectData));
            return;
        }

        if (m_CurrentState.GridStarted)
        {
            EditorGUI::EndPropertyGrid();
            m_CurrentState.GridStarted = false;
        }

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth;
        bool expanded = ImGui::TreeNodeEx(m_CurrentPropertyName.data(), flags);

        if (expanded)
        {
            PropertiesTreeState previousState = m_CurrentState;
            m_CurrentState.GridStarted = false;

            descriptor.Callback(objectData, *this);

            if (m_CurrentState.GridStarted)
                EditorGUI::EndPropertyGrid();

            ImGui::TreePop();
            m_CurrentState = previousState;
        }
    }

    void SerializablePropertyRenderer::SerializeReference(const SerializableObjectDescriptor& valueDescriptor, void* referenceData, void* valueData)
    {
    }

    void SerializablePropertyRenderer::RenderAssetField(AssetHandle& handle)
    {
        EditorGUI::AssetField(handle);
    }

    void SerializablePropertyRenderer::BeginPropertiesGridIfNeeded()
    {
        if (!m_CurrentState.GridStarted)
        {
            EditorGUI::BeginPropertyGrid();
            m_CurrentState.GridStarted = true;
        }
    }
}
