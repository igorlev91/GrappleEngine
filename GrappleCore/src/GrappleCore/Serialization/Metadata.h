#pragma once

#include "GrappleCore/Core.h"
#include "GrappleCore/UUID.h"
#include "GrappleCore/Serialization/Serialization.h"

#include "GrappleCore/Serialization/TypeSerializer.h"

#include <glm/glm.hpp>

namespace Grapple
{
    template<typename T>
    struct SerializablePropertyDataFromType
    {
        constexpr SerializablePropertyDescriptor::InitializationData Get()
        {
            return SerializablePropertyDescriptor::InitializationData(
                SerializablePropertyType::Object,
                0, SerializablePropertyType::None, 
                &Grapple_SERIALIZATION_DESCRIPTOR_OF(T));
        }
    };

#define TYPE_TO_PROPERTY_TYPE(typeName, propertyType) template<>              \
    struct SerializablePropertyDataFromType<typeName>                         \
    {                                                                         \
        constexpr SerializablePropertyDescriptor::InitializationData Get()    \
        {                                                                     \
            return SerializablePropertyDescriptor::InitializationData(        \
                propertyType,                                                 \
                0, SerializablePropertyType::None,                            \
                nullptr);                                                     \
        }                                                                     \
    };

    TYPE_TO_PROPERTY_TYPE(bool, Grapple::SerializablePropertyType::Bool);
    TYPE_TO_PROPERTY_TYPE(float, Grapple::SerializablePropertyType::Float);

    TYPE_TO_PROPERTY_TYPE(int8_t, Grapple::SerializablePropertyType::Int8);
    TYPE_TO_PROPERTY_TYPE(uint8_t, Grapple::SerializablePropertyType::UInt8);
    TYPE_TO_PROPERTY_TYPE(int16_t, Grapple::SerializablePropertyType::Int16);
    TYPE_TO_PROPERTY_TYPE(uint16_t, Grapple::SerializablePropertyType::UInt16);
    TYPE_TO_PROPERTY_TYPE(int32_t, Grapple::SerializablePropertyType::Int32);
    TYPE_TO_PROPERTY_TYPE(uint32_t, Grapple::SerializablePropertyType::UInt32);
    TYPE_TO_PROPERTY_TYPE(int64_t, Grapple::SerializablePropertyType::Int64);
    TYPE_TO_PROPERTY_TYPE(uint64_t, Grapple::SerializablePropertyType::UInt64);

    TYPE_TO_PROPERTY_TYPE(glm::vec2, Grapple::SerializablePropertyType::FloatVector2);
    TYPE_TO_PROPERTY_TYPE(glm::vec3, Grapple::SerializablePropertyType::FloatVector3);
    TYPE_TO_PROPERTY_TYPE(glm::vec4, Grapple::SerializablePropertyType::FloatVector4);

    TYPE_TO_PROPERTY_TYPE(glm::ivec2, Grapple::SerializablePropertyType::IntVector2);
    TYPE_TO_PROPERTY_TYPE(glm::ivec3, Grapple::SerializablePropertyType::IntVector3);
    TYPE_TO_PROPERTY_TYPE(glm::ivec4, Grapple::SerializablePropertyType::IntVector4);

    TYPE_TO_PROPERTY_TYPE(glm::uvec2, Grapple::SerializablePropertyType::UIntVector2);
    TYPE_TO_PROPERTY_TYPE(glm::uvec3, Grapple::SerializablePropertyType::UIntVector3);
    TYPE_TO_PROPERTY_TYPE(glm::uvec4, Grapple::SerializablePropertyType::UIntVector4);
    
    TYPE_TO_PROPERTY_TYPE(std::string, Grapple::SerializablePropertyType::String);
    TYPE_TO_PROPERTY_TYPE(UUID, Grapple::SerializablePropertyType::UUID);

#undef TYPE_TO_PROPERTY_TYPE

    template<typename T, size_t N>
    struct SerializablePropertyDataFromType<T[N]>
    {
        constexpr SerializablePropertyDescriptor::InitializationData Get()
        {
            SerializablePropertyDescriptor::InitializationData elementInfo = SerializablePropertyDataFromType<T>().Get();
            return SerializablePropertyDescriptor::InitializationData(SerializablePropertyType::Array, N, elementInfo.Type, elementInfo.ObjectType);
        }
    };

    template<typename T>
    struct ColorTypeFromDataType
    {
        constexpr SerializablePropertyType GetType()
        {
            static_assert(false, "Not supported color data type");
            return SerializablePropertyType::None;
        }
    };

    template<>
    struct ColorTypeFromDataType<glm::vec3>
    {
        constexpr SerializablePropertyType GetType()
        {
            return SerializablePropertyType::Color3;
        }
    };

    template<>
    struct ColorTypeFromDataType<glm::vec4>
    {
        constexpr SerializablePropertyType GetType()
        {
            return SerializablePropertyType::Color4;
        }
    };

#define Grapple_SERIALIZABLE \
    static Grapple::SerializableObjectDescriptor _SerializationDescriptor;

#define Grapple_SERIALIZABLE_IMPL(typeName, ...) \
    Grapple::SerializableObjectDescriptor typeName::_SerializationDescriptor( \
        typeid(typeName).name(), sizeof(typeName), { __VA_ARGS__ },         \
        [](void* object, SerializationStream& stream) { Grapple::TypeSerializer<typeName>().OnSerialize(*(typeName*)object, stream); });

#define Grapple_PROPERTY(typeName, propertyName)                                                        \
    Grapple::SerializablePropertyDescriptor(                                                            \
        #propertyName, offsetof(typeName, propertyName),                                              \
        Grapple::SerializablePropertyDataFromType<Grapple_TYPE_OF_FIELD(typeName, propertyName)>().Get())

#define Grapple_ENUM_PROPERTY(typeName, propertyName)                                                                           \
    Grapple::SerializablePropertyDescriptor(                                                                                    \
        #propertyName, offsetof(typeName, propertyName),                                                                      \
        Grapple::SerializablePropertyDataFromType<std::underlying_type_t<Grapple_TYPE_OF_FIELD(typeName, propertyName)>>().Get())

#define Grapple_COLOR_PROPERTY(typeName, propertyName)                                                                          \
    Grapple::SerializablePropertyDescriptor(                                                                                    \
        #propertyName, offsetof(typeName, propertyName),                                                                      \
        ColorTypeFromDataType<Grapple_TYPE_OF_FIELD(typeName, propertyName)>().GetType())
}