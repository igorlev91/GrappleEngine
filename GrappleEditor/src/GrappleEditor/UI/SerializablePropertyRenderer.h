#pragma once

#include "GrappleCore/Serialization/SerializationStream.h"

#include "Grapple/AssetManager/Asset.h"

#include "GrappleECS/World.h"
#include "GrappleECS/Entity/Entity.h"

#include <string_view>
#include <vector>

namespace Grapple
{
    class SerializablePropertyRenderer : public SerializationStream
    {
    public:
        SerializablePropertyRenderer(const World* currentWorld = nullptr);

        void PropertyKey(std::string_view key) override;
        DynamicArrayAction SerializeDynamicArraySize(size_t& size) override;
        void SerializeInt(SerializationValue<uint8_t> intValues, SerializableIntType type) override;
        void SerializeBool(SerializationValue<bool> value) override;
        void SerializeFloat(SerializationValue<float> value) override;
        void SerializeUUID(SerializationValue<UUID> uuids) override;
        void SerializeFloatVector(SerializationValue<float> value, uint32_t componentsCount) override;
        void SerializeIntVector(SerializationValue<int32_t> value, uint32_t componentsCount) override;
        void SerializeString(SerializationValue<std::string> value) override;
        void SerializeObject(const SerializableObjectDescriptor& descriptor, void* objectData) override;

        void SerializeReference(const SerializableObjectDescriptor& valueDescriptor,
            void* referenceData,
            void* valueData) override;

        inline bool PropertiesGridStarted() const { return m_CurrentState.GridStarted; }
    private:
        void RenderAssetField(AssetHandle& handle);
    private:
        struct PropertiesTreeState
        {
            bool GridStarted;
        };

        void BeginPropertiesGridIfNeeded();
    private:
        PropertiesTreeState m_CurrentState;
        std::string_view m_CurrentPropertyName;

        const World* m_CurrentWorld;
    };
}