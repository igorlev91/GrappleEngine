#pragma once

#include "Grapple/Renderer/UniformBuffer.h"

#include "Grapple/Platform/Vulkan/VulkanBuffer.h"

namespace Grapple
{
	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(size_t size);

		void SetData(const void* data, size_t size, size_t offset) override;
		size_t GetSize() const override;

		inline VkBuffer GetBufferHandle() const { return m_Buffer.GetBuffer(); }
	private:
		VulkanBuffer m_Buffer;
	};
}
