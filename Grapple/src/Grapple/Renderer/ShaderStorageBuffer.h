#pragma once

#include "GrappleCore/Collections/Span.h"

#include <stdint.h>

namespace Grapple
{
    class CommandBuffer;
    class Grapple_API ShaderStorageBuffer
    {
    public:
        virtual ~ShaderStorageBuffer() {}

        virtual size_t GetSize() const = 0;
        virtual void SetData(const MemorySpan& data) = 0;
        virtual void SetData(const MemorySpan& data, size_t offset, Ref<CommandBuffer> commandBuffer) = 0;

        virtual void Resize(size_t size) = 0;

		virtual void SetDebugName(std::string_view debugName) = 0;
		virtual const std::string& GetDebugName() const = 0;
    public:
        static Ref<ShaderStorageBuffer> Create(size_t size);
    };
}
