#pragma once

#include "GrappleScriptingCore/Defines.h"

#include <string>
#include <string_view>
#include <vector>
#include <typeinfo>
#include <iostream>

namespace Grapple
{
    class ScriptingType
    {
    public:
        using ConstructorFunction = void*(*)();
        using DestructorFunction = void(*)(void*);

        ScriptingType(std::string_view name, size_t size, ConstructorFunction constructor, DestructorFunction destructor)
            : Name(name), Size(size), Constructor(constructor), Destructor(destructor)
        {
            GetRegisteredTypes().push_back(this);
        }

        const std::string_view Name;
        const size_t Size;
        const ConstructorFunction Constructor;
        const DestructorFunction Destructor;
    public:
        static std::vector<const ScriptingType*>& GetRegisteredTypes();
    };
}

#define Grapple_DEFINE_SCRIPTING_TYPE(type) public:   \
    static Grapple::ScriptingType GrappleScriptingType; \
    static void* CreateInstance();                  \
    static void DeleteInstance(void* instance);

#define Grapple_IMPL_SCRIPTING_TYPE(type)                                         \
    void* type::CreateInstance() { return new type(); }                         \
    void type::DeleteInstance(void* instance) { delete (type*)instance; }       \
    Grapple::ScriptingType type::GrappleScriptingType = Grapple::ScriptingType(		\
        typeid(type).name(), 													\
        sizeof(type), 															\
        type::CreateInstance,													\
        type::DeleteInstance);