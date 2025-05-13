#include "OpenGLBuffer.h"

namespace Grapple
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(size_t size)
	{
		glCreateBuffers(1, &m_Id);
		glBindBuffer(GL_ARRAY_BUFFER, m_Id);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(size_t size, const void* data)
	{
		glCreateBuffers(1, &m_Id);
		glBindBuffer(GL_ARRAY_BUFFER, m_Id);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}
	
	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_Id);
	}

	void OpenGLVertexBuffer::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Id);
	}
	
	void OpenGLVertexBuffer::SetData(const void* data, size_t size, size_t offset)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Id);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
	}



	OpenGLIndexBuffer::OpenGLIndexBuffer(IndexFormat format, size_t size)
	{
		size_t indexSize = 0;
		switch (format)
		{
		case IndexFormat::UInt16:
			indexSize = sizeof(uint16_t);
			break;
		case IndexFormat::UInt32:
			indexSize = sizeof(uint32_t);
			break;
		}

		glGenBuffers(1, &m_Id);
		glBindBuffer(GL_ARRAY_BUFFER, m_Id);
		glBufferData(GL_ARRAY_BUFFER, size * indexSize, nullptr, GL_DYNAMIC_DRAW);

		m_Format = format;
		m_SizeInBytes = size;
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(IndexFormat format, const MemorySpan& indices)
	{
		glGenBuffers(1, &m_Id);
		glBindBuffer(GL_ARRAY_BUFFER, m_Id);
		glBufferData(GL_ARRAY_BUFFER, indices.GetSize(), indices.GetBuffer(), GL_STATIC_DRAW);

		m_SizeInBytes = indices.GetSize();
		m_Format = format;
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_Id);
	}

	void OpenGLIndexBuffer::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
	}

	void OpenGLIndexBuffer::SetData(const MemorySpan& indices, size_t offset)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, indices.GetSize(), indices.GetBuffer());
	}

	size_t OpenGLIndexBuffer::GetCount() const
	{
		switch (m_Format)
		{
		case IndexBuffer::IndexFormat::UInt16:
			return m_SizeInBytes / sizeof(uint16_t);
		case IndexBuffer::IndexFormat::UInt32:
			return m_SizeInBytes / sizeof(uint32_t);
		}

		Grapple_CORE_ASSERT(false);
		return 0;
	}

	IndexBuffer::IndexFormat OpenGLIndexBuffer::GetIndexFormat() const
	{
		return m_Format;
	}
}