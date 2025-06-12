#pragma once

#include "GrappleCore/Collections/Span.h"

#include "Grapple/Renderer/Buffer.h"
#include "Grapple/Platform/Vulkan/VulkanAllocation.h"
#include "Grapple/Platform/Vulkan/VulkanContext.h"

#include <vulkan/vulkan.h>

namespace Grapple
{
	class CommandBuffer;
	class VulkanBuffer
	{
	public:
		struct PipelineDependecy
		{
			PipelineDependecy(VkPipelineStageFlags stages, VkAccessFlags accessFlags)
				: DependentStages(stages), AccessFlags(accessFlags) {}

			VkPipelineStageFlags DependentStages = VK_PIPELINE_STAGE_NONE;
			VkAccessFlags AccessFlags = VK_ACCESS_NONE;
		};

		VulkanBuffer(GPUBufferUsage usage, VkBufferUsageFlags bufferUsage, PipelineDependecy dependecy, size_t size);
		VulkanBuffer(GPUBufferUsage usage, VkBufferUsageFlags bufferUsage, PipelineDependecy dependeny);
		~VulkanBuffer();

		void SetData(const void* data, size_t size, size_t offset);
		void SetData(MemorySpan data, size_t offset, Ref<CommandBuffer> commandBuffer);
		void EnsureAllocated();

		inline size_t GetSize() const { return m_Size; }
		inline VkBuffer GetBuffer() const { return m_Buffer; }

		void SetDebugName(std::string_view name);
		inline const std::string& GetDebugName() const { return m_DebugName; }
	private:
		void Create();
		VulkanStagingBuffer FillStagingBuffer(MemorySpan data);
	protected:
		std::string m_DebugName;

		PipelineDependecy m_PipelineDepency;

		GPUBufferUsage m_Usage = GPUBufferUsage::Static;
		void* m_Mapped = nullptr;

		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VulkanAllocation m_Allocation;

		size_t m_Size = 0;

		VkBufferUsageFlags m_UsageFlags = 0;
	};
}
