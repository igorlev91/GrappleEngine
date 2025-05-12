#pragma once

namespace Grapple
{
    class SerializationStream;
    class SerializableObjectDescriptor;

    template<typename T>
    struct TypeSerializer
    {
        void OnSerialize(T& value, SerializationStream& stream) {}
    };

    template<typename T>
    struct SerializationDescriptorOf
    {
        static constexpr const SerializableObjectDescriptor* Descriptor()
        {
            return &T::_SerializationDescriptor;
        }
    };
}