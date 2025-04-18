#pragma once

#include "Grapple/Renderer/UniformBuffer.h"

namespace Grapple
{
	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(size_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer();

		void SetData(const void* data, size_t size, size_t offset) override;
	private:
		uint32_t m_Id;
	};
}