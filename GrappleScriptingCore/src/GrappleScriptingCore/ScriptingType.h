#pragma once

#include "GrappleScriptingCore/TypeSerializationSettings.h"
#include "Grapple/Core/Core.h"
#include "Grapple/Core/Assert.h"

#include <string>
#include <string_view>
#include <vector>
#include <typeinfo>
#include <iostream>

namespace Grapple::Internal
{
    class ScriptingType
    {
    public:
        using ConstructorFunction = void*(*)();
        using DestructorFunction = void(*)(void*);
        using ConfigureSerializationFunction = void(*)(TypeSerializationSettings&);

        ScriptingType(std::string_view name, size_t size,
            ConstructorFunction constructor,
            DestructorFunction destructor,
            DestructorFunction deleter,
            ConfigureSerializationFunction configureSerialization)
            : Name(name), Size(size),
                Constructor(constructor),
                Destructor(destructor),
                Deleter(deleter),
                ConfigureSerialization(configureSerialization)
        {
            GetRegisteredTypes().push_back(this);
        }

        TypeSerializationSettings& GetSerializationSettings() { return m_SerializationSettings; }
        const TypeSerializationSettings& GetSerializationSettings() const { return m_SerializationSettings; }

        const std::string_view Name;
        const size_t Size;

        const ConstructorFunction Constructor;
        const DestructorFunction Destructor;
        const DestructorFunction Deleter;
        const ConfigureSerializationFunction ConfigureSerialization;
    private:
        TypeSerializationSettings m_SerializationSettings;
    public:
        static std::vector<ScriptingType*>& GetRegisteredTypes();
    };
}

#define Grapple_DEFINE_SCRIPTING_TYPE(type) public:   \
    static Grapple::ScriptingType GrappleScriptingType; \
    static void* CreateInstance();                  \
    static void DeleteInstance(void* instance);     \
    static void DestroyInstance(void* instance);

#define Grapple_IMPL_SCRIPTING_TYPE_FULL(type, configureSerialization, ...)       \
    void* type::CreateInstance() { return new type(); }                         \
    void type::DeleteInstance(void* instance) { delete (type*)instance; }       \
    void type::DestroyInstance(void* instance) { ((type*)instance)->~type(); }  \
    Grapple::ScriptingType type::GrappleScriptingType = Grapple::ScriptingType(		\
        typeid(type).name(), 													\
        sizeof(type), 															\
        type::CreateInstance,													\
        type::DestroyInstance,                                                  \
        type::DeleteInstance, configureSerialization);

#define Grapple_SCRIPTING_TYPE_GET_IMPL_ARGS(type, serializationConfig, ...) Grapple_IMPL_SCRIPTING_TYPE_FULL(type, serializationConfig)
#define Grapple_IMPL_SCRIPTING_TYPE(...) Grapple_EXPEND_MACRO(Grapple_SCRIPTING_TYPE_GET_IMPL_ARGS(__VA_ARGS__, 0))