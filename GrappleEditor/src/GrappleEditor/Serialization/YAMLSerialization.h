#pragma once

#include "GrappleCore/Serialization/SerializationStream.h"
#include "GrappleCore/UUID.h"

#include "GrappleECS/World.h"
#include "GrappleECS/Entity/Entity.h"

#include <yaml-cpp/yaml.h>
#include <unordered_map>

namespace Grapple
{
    class YAMLSerializer : public SerializationStream
    {
    public:
        YAMLSerializer(YAML::Emitter& emitter, const World& world);
    public:
        void PropertyKey(std::string_view key) override;
        DynamicArrayAction SerializeDynamicArraySize(size_t& size) override;
        void SerializeInt(SerializationValue<uint8_t> intValues, SerializableIntType type) override;
        void SerializeBool(SerializationValue<bool> value) override;
        void SerializeFloat(SerializationValue<float> value) override;
        void SerializeUUID(SerializationValue<UUID> uuids) override;
        void SerializeFloatVector(SerializationValue<float> value, uint32_t componentsCount) override;
        void SerializeIntVector(SerializationValue<int32_t> value, uint32_t componentsCount) override;
        void SerializeString(SerializationValue<std::string> value) override;
        void SerializeObject(const SerializableObjectDescriptor& descriptor, void* objectData, bool isArray, size_t arraySize) override;

        void SerializeReference(const SerializableObjectDescriptor& valueDescriptor,
            void* referenceData,
            void* valueData) override;
    public:
        YAML::Emitter& m_Emitter;
        const World& m_World;
        bool m_ObjectSerializationStarted;
        bool m_MapStarted;
    };

    class YAMLDeserializer : public SerializationStream
    {
    public:
        YAMLDeserializer(const YAML::Node& root, std::unordered_map<UUID, Entity>* serializationIdToECSId = nullptr);

        void PropertyKey(std::string_view key) override;
        DynamicArrayAction SerializeDynamicArraySize(size_t& size) override;
        void SerializeInt(SerializationValue<uint8_t> intValues, SerializableIntType type) override;
        void SerializeBool(SerializationValue<bool> value) override;
        void SerializeFloat(SerializationValue<float> value) override;
        void SerializeUUID(SerializationValue<UUID> uuids) override;
        void SerializeFloatVector(SerializationValue<float> value, uint32_t componentsCount) override;
        void SerializeIntVector(SerializationValue<int32_t> value, uint32_t componentsCount) override;
        void SerializeString(SerializationValue<std::string> value) override;
        void SerializeObject(const SerializableObjectDescriptor& descriptor, void* objectData, bool isArray, size_t arraySize) override;

        void SerializeReference(const SerializableObjectDescriptor& valueDescriptor,
            void* referenceData,
            void* valueData) override;
    private:
        inline YAML::Node& CurrentNode()
        {
            Grapple_CORE_ASSERT(m_NodesStack.size() > 0);
            return m_NodesStack.back();
        }
    private:
        const YAML::Node& m_Root;
        std::unordered_map<UUID, Entity>* m_SerializationIdToECSId;

        std::string m_CurrentPropertyKey;
        std::vector<YAML::Node> m_NodesStack;
    };
}