#pragma once

#include "Grapple/Core/Core.h"

#include <glm/glm.hpp>

#include <vector>
#include <string_view>

namespace Grapple
{
    enum class SerializableFieldType : uint8_t
    {
        None,

        Bool,

        Int8,
        UInt8,

        Int16,
        UInt16,

        Int32,
        UInt32,

        Int64,
        UInt64,

        Float32,
        Float64,

        Float2,
        Float3,
        Float4,

        Int2,
        Int3,
        Int4,

        UInt2,
        UInt3,
        UInt4,

        Custom,
    };

    class GrappleCOMMON_API TypeInitializer;
    struct FieldData
    {
        constexpr FieldData(const char* name, SerializableFieldType fieldType, size_t offset, TypeInitializer* type)
            : Name(name), FieldType(fieldType), Offset(offset), Type(type) {}

        const char* Name;
        SerializableFieldType FieldType;
        size_t Offset;
        TypeInitializer* Type;
    };

    template<typename T>
    constexpr FieldData GetFieldData(const char* fieldName, size_t offset)
    {
        if constexpr (std::is_same<T, float>().value)
            return FieldData{ fieldName, SerializableFieldType::Float32, offset, nullptr };
        if constexpr (std::is_same<T, double>().value)
            return FieldData{ fieldName, SerializableFieldType::Float64, offset, nullptr };
        return FieldData{ fieldName, SerializableFieldType::Custom, offset, &T::_Type };
    }

#define GET_FIELD_DATA(typeName, fieldType) template<> constexpr Grapple::FieldData GetFieldData<typeName>(const char* name, size_t offset) \
    { \
        return FieldData{name, fieldType, offset, nullptr}; \
    }

    GET_FIELD_DATA(bool, SerializableFieldType::Bool);

    GET_FIELD_DATA(int8_t,   SerializableFieldType::Int8);
    GET_FIELD_DATA(uint8_t,  SerializableFieldType::UInt8);
    GET_FIELD_DATA(int16_t,  SerializableFieldType::Int16);
    GET_FIELD_DATA(uint16_t, SerializableFieldType::UInt16);
    GET_FIELD_DATA(int32_t,  SerializableFieldType::Int32);
    GET_FIELD_DATA(uint32_t, SerializableFieldType::UInt32);
    GET_FIELD_DATA(int64_t,  SerializableFieldType::Int64);
    GET_FIELD_DATA(uint64_t, SerializableFieldType::UInt64);

    GET_FIELD_DATA(glm::vec2, SerializableFieldType::Float2);
    GET_FIELD_DATA(glm::vec3, SerializableFieldType::Float3);
    GET_FIELD_DATA(glm::vec4, SerializableFieldType::Float4);

    GET_FIELD_DATA(glm::ivec2, SerializableFieldType::Int2);
    GET_FIELD_DATA(glm::ivec3, SerializableFieldType::Int3);
    GET_FIELD_DATA(glm::ivec4, SerializableFieldType::Int4);

    GET_FIELD_DATA(glm::uvec2, SerializableFieldType::UInt2);
    GET_FIELD_DATA(glm::uvec3, SerializableFieldType::UInt3);
    GET_FIELD_DATA(glm::uvec4, SerializableFieldType::UInt4);

#undef GET_FIELD_DATA

#define Grapple_GET_FIELD_TYPE(typeName, fieldName) decltype(((typeName*)nullptr)->fieldName)
#define Grapple_FIELD(typeName, fieldName) Grapple::GetFieldData<Grapple_GET_FIELD_TYPE(typeName, fieldName)>(#fieldName, offsetof(typeName, fieldName))

    class GrappleCOMMON_API TypeInitializer
    {
    public:
        using DefaultConstructorFunction = void(*)(void*);
        using DestructorFunction = void(*)(void*);

        TypeInitializer(std::string_view typeName, size_t size, 
            DestructorFunction destructor, 
            DefaultConstructorFunction constructor, 
            const std::initializer_list<FieldData>& fields);
        ~TypeInitializer();

        static std::vector<TypeInitializer*>& GetInitializers();
    public:
        const std::string_view TypeName;
        const std::vector<FieldData> SerializedFields;
        const DestructorFunction Destructor;
        const DefaultConstructorFunction DefaultConstructor;
        const size_t Size;
    };
}

#define Grapple_TYPE static Grapple::TypeInitializer _Type;
#define Grapple_IMPL_TYPE(typeName, ...) Grapple::TypeInitializer typeName::_Type =   \
    Grapple::TypeInitializer(typeid(typeName).name(), sizeof(typeName),             \
    [](void* instance) { ((typeName*)instance)->~typeName(); },                   \
    [](void* instance) { new(instance) typeName;},                                \
    { __VA_ARGS__ })