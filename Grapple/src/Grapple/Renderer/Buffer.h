#pragma once

#include "GrappleCore/Core.h"

#include "Grapple/Renderer/ShaderMetadata.h"

#include <stdint.h>
#include <string>
#include <string_view>
#include <vector>

namespace Grapple
{
	class CommandBuffer;

	enum class GPUBufferUsage
	{
		Static = 0,
		Dynamic = 1,
	};

	class Grapple_API VertexBuffer
	{
	public:
		virtual void Bind() = 0;
		virtual void SetData(const void* data, size_t size, size_t offset = 0) = 0;
		virtual void SetData(MemorySpan data, size_t offset, Ref<CommandBuffer> commandBuffer) = 0;
	public:
		static Ref<VertexBuffer> Create(size_t size);
		static Ref<VertexBuffer> Create(size_t size, const void* data);

		// commandBuffer - a CommandBuffer used for submitting copy commands
		static Ref<VertexBuffer> Create(size_t size, const void* data, Ref<CommandBuffer> commandBuffer);
	};

	class Grapple_API IndexBuffer
	{
	public:
		enum class IndexFormat
		{
			UInt32,
			UInt16,
		};

		virtual void Bind() = 0;
		virtual void SetData(const MemorySpan& indices, size_t offset = 0) = 0;
		virtual void SetData(MemorySpan data, size_t offset, Ref<CommandBuffer> commandBuffer) = 0;
		
		virtual size_t GetCount() const = 0;
		virtual IndexFormat GetIndexFormat() const = 0;
	public:
		static size_t GetIndexFormatSize(IndexFormat format);
		static Ref<IndexBuffer> Create(IndexFormat format, size_t size);
		static Ref<IndexBuffer> Create(IndexFormat format, const MemorySpan& indices);

		// commandBuffer - a CommandBuffer used for submitting copy commands
		static Ref<IndexBuffer> Create(IndexFormat format, const MemorySpan& indices, Ref<CommandBuffer> commandBuffer);
	};
}