#pragma once

#include "GrappleCore/Core.h"

#include "Grapple/Renderer/ShaderMetadata.h"

#include <stdint.h>
#include <string>
#include <string_view>
#include <vector>

namespace Grapple
{
	struct BufferLayoutElement
	{
		BufferLayoutElement() = default;
		BufferLayoutElement(std::string_view name, ShaderDataType type, bool normalized = false)
			: Name(name), DataType(type), Size(ShaderDataTypeSize(type)), 
			ComponentsCount(ShaderDataTypeComponentCount(type)), IsNormalized(normalized), Offset(0) {}

		std::string Name;
		ShaderDataType DataType;
		uint32_t Size;
		uint32_t Offset;
		uint32_t ComponentsCount;
		bool IsNormalized;
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferLayoutElement>& elements)
			: m_Elements(elements), m_Stride(0)
		{
			CalculateOffsetsAndStrides();
		}

		inline const std::vector<BufferLayoutElement> GetElements() const { return m_Elements; }
		inline uint32_t GetStride() const { return m_Stride; }
	private:
		void CalculateOffsetsAndStrides()
		{
			for (auto& elem : m_Elements)
			{
				elem.Offset = m_Stride;
				m_Stride += elem.Size;
			}
		}
	private:
		uint32_t m_Stride;
		std::vector<BufferLayoutElement> m_Elements;
	};

	class Grapple_API VertexBuffer
	{
	public:
		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		virtual void Bind() = 0;
		virtual void SetData(const void* data, size_t size, size_t offset = 0) = 0;
	public:
		static Ref<VertexBuffer> Create(size_t size);
		static Ref<VertexBuffer> Create(size_t size, const void* data);
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
		
		virtual size_t GetCount() const = 0;
		virtual IndexFormat GetIndexFormat() const = 0;
	public:
		static Ref<IndexBuffer> Create(IndexFormat format, size_t size);
		static Ref<IndexBuffer> Create(IndexFormat format, const MemorySpan& indices);
	};
}