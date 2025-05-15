#include "YAMLSerialization.h"

#include "Grapple/Serialization/Serialization.h"
#include "GrappleEditor/Serialization/SerializationId.h"

namespace Grapple
{
    YAMLSerializer::YAMLSerializer(YAML::Emitter& emitter, const World& world)
        : m_Emitter(emitter), m_MapStarted(false), m_ObjectSerializationStarted(false), m_World(world) {}

    void YAMLSerializer::PropertyKey(std::string_view key)
    {
        if (!m_MapStarted && m_ObjectSerializationStarted)
        {
            m_Emitter << YAML::BeginMap;
            m_MapStarted = true;
        }

        m_Emitter << YAML::Key << std::string(key);
    }

    SerializationStream::DynamicArrayAction YAMLSerializer::SerializeDynamicArraySize(size_t& size)
    {
        return DynamicArrayAction::None;
    }

    void YAMLSerializer::SerializeInt(SerializationValue<uint8_t> intValues, SerializableIntType type)
    {
        if (intValues.IsArray)
            m_Emitter << YAML::BeginSeq;

        size_t intSize = SizeOfSerializableIntType(type);
        for (size_t i = 0; i < intValues.Values.GetSize(); i += intSize)
        {
            m_Emitter << YAML::Value;
            
#define SERIALIZE_INT(intType, serializableIntType)                           \
            case SerializableIntType::serializableIntType:                    \
                m_Emitter << reinterpret_cast<intType&>(intValues.Values[i]); \
                break;

            switch (type)
            {
            case SerializableIntType::Int8:
                m_Emitter << (int16_t)reinterpret_cast<int8_t&>(intValues.Values[i]);
                break;
            case SerializableIntType::UInt8:
                m_Emitter << (uint16_t)reinterpret_cast<uint8_t&>(intValues.Values[i]);
                break;

                SERIALIZE_INT(int16_t, Int16);
                SERIALIZE_INT(uint16_t, UInt16);
                SERIALIZE_INT(int32_t, Int32);
                SERIALIZE_INT(uint32_t, UInt32);
                SERIALIZE_INT(int64_t, Int64);
                SERIALIZE_INT(uint64_t, UInt64);
            }

#undef SERIALIZE_INT
        }

        if (intValues.IsArray)
            m_Emitter << YAML::EndSeq;
    }

    template<typename T>
    static void SerializeValue(YAML::Emitter& emitter, SerializationValue<T>& value)
    {
        if (value.IsArray)
            emitter << YAML::BeginSeq;

        for (size_t i = 0; i < value.Values.GetSize(); i++)
            emitter << YAML::Value << value.Values[i];

        if (value.IsArray)
            emitter << YAML::EndSeq;
    }

    void YAMLSerializer::SerializeBool(SerializationValue<bool> value)
    {
        SerializeValue(m_Emitter, value);
    }

    void YAMLSerializer::SerializeFloat(SerializationValue<float> value)
    {
        SerializeValue(m_Emitter, value);
    }

    void YAMLSerializer::SerializeUUID(SerializationValue<UUID> uuids)
    {
        SerializeValue(m_Emitter, uuids);
    }

    void YAMLSerializer::SerializeFloatVector(SerializationValue<float> value, uint32_t componentsCount)
    {
        if (value.IsArray)
            m_Emitter << YAML::BeginSeq;

        for (size_t i = 0; i < value.Values.GetSize(); i += (size_t)componentsCount)
        {
            m_Emitter << YAML::Value;
            float* vectorValues = &value.Values[i];
            switch (componentsCount)
            {
            case 1:
                m_Emitter << *vectorValues;
                break;
            case 2:
                m_Emitter << glm::vec2(vectorValues[0], vectorValues[1]);
                break;
            case 3:
                m_Emitter << glm::vec3(vectorValues[0], vectorValues[1], vectorValues[2]);
                break;
            case 4:
                m_Emitter << glm::vec4(vectorValues[0], vectorValues[1], vectorValues[2], vectorValues[3]);
                break;
            }
        }

        if (value.IsArray)
            m_Emitter << YAML::EndSeq;
    }

    void YAMLSerializer::SerializeIntVector(SerializationValue<int32_t> value, uint32_t componentsCount)
    {
        if (value.IsArray)
            m_Emitter << YAML::BeginSeq;

        for (size_t i = 0; i < value.Values.GetSize(); i += (size_t)componentsCount)
        {
            m_Emitter << YAML::Value;
            int32_t* vectorValues = &value.Values[i];
            switch (componentsCount)
            {
            case 1:
                m_Emitter << *vectorValues;
                break;
            case 2:
                m_Emitter << glm::ivec2(vectorValues[0], vectorValues[1]);
                break;
            case 3:
                m_Emitter << glm::ivec3(vectorValues[0], vectorValues[1], vectorValues[2]);
                break;
            case 4:
                m_Emitter << glm::ivec4(vectorValues[0], vectorValues[1], vectorValues[2], vectorValues[3]);
                break;
            }
        }

        if (value.IsArray)
            m_Emitter << YAML::EndSeq;
    }

    void YAMLSerializer::SerializeString(SerializationValue<std::string> value)
    {
        if (value.IsArray)
            m_Emitter << YAML::BeginSeq;

        for (size_t i = 0; i < value.Values.GetSize(); i++)
            m_Emitter << YAML::Value << std::string(value.Values[i]);

        if (value.IsArray)
            m_Emitter << YAML::EndSeq;
    }

    void YAMLSerializer::SerializeObject(const SerializableObjectDescriptor& descriptor, void* objectData)
    {
        if (&descriptor == &Grapple_SERIALIZATION_DESCRIPTOR_OF(AssetHandle))
        {
            m_Emitter << YAML::Value << *(AssetHandle*)(objectData);
            return;
        }

        if (&descriptor == &Grapple_SERIALIZATION_DESCRIPTOR_OF(Entity))
        {
            Entity entityId = *(Entity*)objectData;

            if (m_World.IsEntityAlive(entityId))
            {
                std::optional<const SerializationId*> id = m_World.TryGetEntityComponent<SerializationId>(entityId);
                if (id)
                {
                    m_Emitter << YAML::Value << id.value()->Id;
                    return;
                }
            }

            m_Emitter << YAML::Value << 0;
            return;
        }

        bool previousObjectSerializationState = m_ObjectSerializationStarted;
        bool previousState = m_MapStarted;

        m_ObjectSerializationStarted = true;
        m_MapStarted = false;

        // NOTE: Map starts when the serializer recieves a property with a key,
        //       this prevents YAML-cpp from generating an invalid one when an
        //       object serializes it's properties without any keys
        descriptor.Callback(objectData, *this);

        if (m_MapStarted)
            m_Emitter << YAML::EndMap;

        m_MapStarted = previousState;
        m_ObjectSerializationStarted = previousObjectSerializationState;
    }

    void YAMLSerializer::SerializeReference(const SerializableObjectDescriptor& valueDescriptor, void* referenceData, void* valueData)
    {
        Grapple_CORE_WARN("YAMLSerializer: Reference serialization not supported");
    }

    // YAML Deserializer

    YAMLDeserializer::YAMLDeserializer(const YAML::Node& root, std::unordered_map<UUID, Entity>* serializationIdToECSId)
        : m_Root(root), m_SerializationIdToECSId(serializationIdToECSId)
    {
        m_NodesStack.push_back(m_Root);
    }

    void YAMLDeserializer::PropertyKey(std::string_view key)
    {
        m_CurrentPropertyKey = key;
    }

    SerializationStream::DynamicArrayAction YAMLDeserializer::SerializeDynamicArraySize(size_t& size)
    {
        size = CurrentNode().size();
        return DynamicArrayAction::Resize;
    }

    static void DeserializeIntValue(uint8_t* outValue, const YAML::Node& node, SerializableIntType type)
    {
#define DESERIALIZE_INT(intType, serializableIntType)       \
            case SerializableIntType::serializableIntType:  \
                *(intType*)(outValue) = node.as<intType>(); \
                break;

        switch (type)
        {
        case SerializableIntType::Int8:
            (*(int8_t*)outValue) = (int8_t)node.as<int16_t>();
            break;
        case SerializableIntType::UInt8:
            (*(uint8_t*)outValue) = (uint8_t)node.as<uint16_t>();
            break;

            DESERIALIZE_INT(int16_t, Int16);
            DESERIALIZE_INT(uint16_t, UInt16);
            DESERIALIZE_INT(int32_t, Int32);
            DESERIALIZE_INT(uint32_t, UInt32);
            DESERIALIZE_INT(int64_t, Int64);
            DESERIALIZE_INT(uint64_t, UInt64);
        }

#undef DESERIALIZE_INT
    }

    void YAMLDeserializer::SerializeInt(SerializationValue<uint8_t> intValues, SerializableIntType type)
    {
        size_t intSize = SizeOfSerializableIntType(type);
        if (intValues.IsArray)
        {
            size_t index = 0;
            for (const YAML::Node& node : CurrentNode())
            {
                if (index >= intValues.Values.GetSize())
                    break;
                
                DeserializeIntValue(&intValues.Values[index], node, type);
                index += intSize;
            }
        }
        else
        {
            if (YAML::Node node = CurrentNode()[m_CurrentPropertyKey])
                DeserializeIntValue(intValues.Values.GetData(), node, type);
        }
    }

    template<typename T>
    void DeserializeValue(SerializationValue<T>& value, const YAML::Node& currentNode, const std::string& currentPropertyName)
    {
        if (value.IsArray)
        {
            size_t index = 0;
            for (const YAML::Node& node : currentNode)
            {
                if (index >= value.Values.GetSize())
                    break;

                value.Values[index] = node.as<T>();
                index++;
            }
        }
        else
        {
            if (YAML::Node node = currentNode[currentPropertyName])
                value.Values[0] = node.as<T>();
        }
    }

    void YAMLDeserializer::SerializeBool(SerializationValue<bool> value)
    {
        DeserializeValue(value, CurrentNode(), m_CurrentPropertyKey);
    }

    void YAMLDeserializer::SerializeFloat(SerializationValue<float> value)
    {
        DeserializeValue(value, CurrentNode(), m_CurrentPropertyKey);
    }

    void YAMLDeserializer::SerializeUUID(SerializationValue<UUID> uuids)
    {
        DeserializeValue(uuids, CurrentNode(), m_CurrentPropertyKey);
    }

    template<typename T>
    static void DeserializeSingleVector(T* destination, const YAML::Node& node, size_t componentsCount)
    {
        switch (componentsCount)
        {
        case 1:
            *destination = node.as<T>();
            break;
        case 2:
        {
            auto vector = node.as<glm::vec<2, T>>();
            std::memcpy(destination, &vector, sizeof(vector));
            break;
        }
        case 3:
        {
            auto vector = node.as<glm::vec<3, T>>();
            std::memcpy(destination, &vector, sizeof(vector));
            break;
        }
        case 4:
        {
            auto vector = node.as<glm::vec<4, T>>();
            std::memcpy(destination, &vector, sizeof(vector));
            break;
        }
        }
    }

    template<typename T>
    static void DeserializeVector(SerializationValue<T>& value, uint32_t componentsCount, YAML::Node& currentNode, const std::string& currentKey)
    {
        if (!value.IsArray)
        {
            const auto& node = currentNode[currentKey];
            DeserializeSingleVector<T>(&value.Values[0], node, (size_t)componentsCount);
            return;
        }

        size_t vectorIndex = 0;
        for (const YAML::Node& node : currentNode)
        {
            DeserializeSingleVector<T>(&value.Values[vectorIndex], node, (size_t)componentsCount);
            vectorIndex += (size_t)componentsCount;

            if (vectorIndex >= value.Values.GetSize())
                break;
        }
    }

    void YAMLDeserializer::SerializeFloatVector(SerializationValue<float> value, uint32_t componentsCount)
    {
        DeserializeVector<float>(value, componentsCount, CurrentNode(), m_CurrentPropertyKey);
    }

    void YAMLDeserializer::SerializeIntVector(SerializationValue<int32_t> value, uint32_t componentsCount)
    {
        DeserializeVector<int32_t>(value, componentsCount, CurrentNode(), m_CurrentPropertyKey);
    }

    void YAMLDeserializer::SerializeString(SerializationValue<std::string> value)
    {
        DeserializeValue<std::string>(value, CurrentNode(), m_CurrentPropertyKey);
    }

    void YAMLDeserializer::SerializeObject(const SerializableObjectDescriptor& descriptor, void* objectData)
    {
        try
        {
            if (&descriptor == &Grapple_SERIALIZATION_DESCRIPTOR_OF(AssetHandle))
            {
                if (YAML::Node handleNode = CurrentNode()[m_CurrentPropertyKey])
                    (*(AssetHandle*)objectData) = handleNode.as<AssetHandle>();

                return;
            }

            if (&descriptor == &Grapple_SERIALIZATION_DESCRIPTOR_OF(Entity))
            {
                Entity& entityId = *(Entity*)objectData;
                YAML::Node idNode = CurrentNode()[m_CurrentPropertyKey];

                if (idNode && m_SerializationIdToECSId != nullptr)
                {
                    UUID serializationId = idNode.as<UUID>();

                    auto it = m_SerializationIdToECSId->find(serializationId);
                    if (it != m_SerializationIdToECSId->end())
                    {
                        entityId = it->second;
                        return;
                    }
                }

                entityId = Entity();
                return;
            }

            if (YAML::Node objectNode = CurrentNode()[m_CurrentPropertyKey])
            {
                m_NodesStack.push_back(objectNode);
                descriptor.Callback(objectData, *this);
                m_NodesStack.pop_back();
            }
        }
        catch (YAML::BadConversion& e)
        {
            Grapple_CORE_ERROR("Failed to deserialize object Line: {} Col: {} Erorr: {}", e.mark.line, e.mark.column, e.what());
        }
        catch (std::exception& e)
        {
            Grapple_CORE_ERROR("Failed to deserialize object: {}", e.what());
        }
    }

    void YAMLDeserializer::SerializeReference(const SerializableObjectDescriptor& valueDescriptor, void* referenceData, void* valueData)
    {
        Grapple_CORE_WARN("YAMLDeserializer: Reference deserialization not supported");
    }
}
