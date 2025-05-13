#pragma once

#include "GrappleCore/Core.h"
#include "GrappleCore/Serialization/Serialization.h"

#define Grapple_SERIALIZABLE                                              \
    static Grapple::SerializableObjectDescriptor _SerializationDescriptor;

#define Grapple_SERIALIZABLE_IMPL(typeName)                                              \
    Grapple::SerializableObjectDescriptor typeName::_SerializationDescriptor(            \
        typeid(typeName).name(), sizeof(typeName),                                     \
        [](void* object, Grapple::SerializationStream& stream) {                         \
            Grapple::TypeSerializer<typeName>().OnSerialize(*(typeName*)object, stream); \
        });
