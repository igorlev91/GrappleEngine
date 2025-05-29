#pragma once

#include "Grapple/Renderer/ShaderStorageBuffer.h"
#include "Grapple/Platform/Vulkan/VulkanBuffer.h"

namespace Grapple
{
	class VulkanShaderStorageBuffer : public ShaderStorageBuffer
	{
	public:
		VulkanShaderStorageBuffer(size_t size);
	
		size_t GetSize() const override;
		void SetData(const MemorySpan& data) override;

		inline VkBuffer GetBufferHandle() const { return m_Buffer.GetBuffer(); }
	private:
		VulkanBuffer m_Buffer;
	};
}
