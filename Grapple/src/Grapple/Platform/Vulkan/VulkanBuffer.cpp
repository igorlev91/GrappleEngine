#include "VulkanBuffer.h"

#include "Grapple/Platform/Vulkan/VulkanContext.h"
#include "GrappleCore/Profiler/Profiler.h"

namespace Grapple
{
	VulkanBuffer::VulkanBuffer(GPUBufferUsage usage, VkBufferUsageFlags bufferUsage, PipelineDependecy dependecy, size_t size)
		: m_Usage(usage), m_UsageFlags(bufferUsage), m_PipelineDepency(dependecy), m_Size(size)
	{
		if (usage == GPUBufferUsage::Static)
		{
			m_UsageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}
	}

	VulkanBuffer::VulkanBuffer(GPUBufferUsage usage, VkBufferUsageFlags bufferUsage, PipelineDependecy dependency)
		: m_Usage(usage), m_UsageFlags(bufferUsage), m_PipelineDepency(dependency), m_Size(0)
	{
	}

	VulkanBuffer::~VulkanBuffer()
	{
		Grapple_CORE_ASSERT(VulkanContext::GetInstance().IsValid());

		VkDevice device = VulkanContext::GetInstance().GetDevice();

		if (m_Mapped)
		{
			vmaUnmapMemory(VulkanContext::GetInstance().GetMemoryAllocator(), m_Allocation.Handle);
			m_Mapped = nullptr;
		}

		vmaFreeMemory(VulkanContext::GetInstance().GetMemoryAllocator(), m_Allocation.Handle);
		vkDestroyBuffer(device, m_Buffer, nullptr);
	}

	void VulkanBuffer::SetData(const void* data, size_t size, size_t offset)
	{
		Grapple_PROFILE_FUNCTION();
		if (size  == 0)
			return;

		if (m_Size == 0)
			m_Size = size;

		Grapple_CORE_ASSERT(size <= m_Size);

		EnsureAllocated();

		if (m_Usage == GPUBufferUsage::Dynamic)
		{
			Grapple_CORE_ASSERT(m_Mapped);

			std::memcpy((uint8_t*)m_Mapped + offset, data, size);
		}
		else
		{
			VulkanStagingBufferPool& stagingBufferPool = VulkanContext::GetInstance().GetStagingBufferPool();

			Ref<VulkanCommandBuffer> commandBuffer = VulkanContext::GetInstance().BeginTemporaryCommandBuffer();
			VulkanStagingBuffer stagingBuffer = FillStagingBuffer(MemorySpan::FromRawBytes(const_cast<void*>(data), size));

			commandBuffer->CopyBuffer(stagingBuffer.Buffer, m_Buffer, size, stagingBuffer.Offset, offset);
			VulkanContext::GetInstance().EndTemporaryCommandBuffer(commandBuffer);

			stagingBufferPool.ReleaseStagingBuffer(stagingBuffer);
		}
	}

	void VulkanBuffer::SetData(MemorySpan data, size_t offset, Ref<CommandBuffer> commandBuffer)
	{
		Grapple_PROFILE_FUNCTION();
		Grapple_CORE_ASSERT(m_Usage == GPUBufferUsage::Static);

		if (data.GetSize() == 0)
			return;

		if (m_Size == 0)
			m_Size = data.GetSize();

		Grapple_CORE_ASSERT(data.GetSize() + offset <= m_Size);
		Grapple_CORE_ASSERT(data.GetSize() <= m_Size);

		EnsureAllocated();

		VulkanStagingBuffer stagingBuffer = FillStagingBuffer(data);

		Ref<VulkanCommandBuffer> vulkanCommandBuffer = As<VulkanCommandBuffer>(commandBuffer);

		VkBufferMemoryBarrier barriers[2] = {};
		barriers[0].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		barriers[0].buffer = m_Buffer;
		barriers[0].offset = (VkDeviceSize)offset;
		barriers[0].size = m_Size - offset;
		barriers[0].pNext = nullptr;
		barriers[0].srcAccessMask = m_PipelineDepency.AccessFlags;
		barriers[0].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barriers[1].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		barriers[1].buffer = m_Buffer;
		barriers[1].offset = (VkDeviceSize)offset;
		barriers[1].size = m_Size - offset;
		barriers[1].pNext = nullptr;
		barriers[1].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barriers[1].dstAccessMask = m_PipelineDepency.AccessFlags;
		barriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		vulkanCommandBuffer->AddBufferBarrier(Span(&barriers[0], 1), m_PipelineDepency.DependentStages, VK_PIPELINE_STAGE_TRANSFER_BIT);
		vulkanCommandBuffer->CopyBuffer(stagingBuffer.Buffer, m_Buffer, data.GetSize(), stagingBuffer.Offset, offset);
		vulkanCommandBuffer->AddBufferBarrier(Span(&barriers[1], 1), VK_PIPELINE_STAGE_TRANSFER_BIT, m_PipelineDepency.DependentStages);
	}

	void VulkanBuffer::EnsureAllocated()
	{
		if (m_Buffer)
			return;
		
		Create();
	}

	void VulkanBuffer::SetDebugName(std::string_view name)
	{
		m_DebugName = name;
		VulkanContext::GetInstance().SetDebugName(VK_OBJECT_TYPE_BUFFER, (uint64_t)m_Buffer, m_DebugName.c_str());
	}

	void VulkanBuffer::Create()
	{
		Grapple_PROFILE_FUNCTION();
		Grapple_CORE_ASSERT(m_Size > 0);
		Grapple_CORE_ASSERT(m_Buffer == VK_NULL_HANDLE);

		VkBufferCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.flags = 0;
		info.pNext = nullptr;
		info.pQueueFamilyIndices = nullptr;
		info.queueFamilyIndexCount = 0;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.size = m_Size;
		info.usage = m_UsageFlags;

		VmaAllocationCreateInfo allocation{};

		switch (m_Usage)
		{
		case GPUBufferUsage::Static:
			allocation.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
			break;
		case GPUBufferUsage::Dynamic:
			allocation.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
			allocation.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
			break;
		}

		VK_CHECK_RESULT(vmaCreateBuffer(VulkanContext::GetInstance().GetMemoryAllocator(), &info, &allocation, &m_Buffer, &m_Allocation.Handle, &m_Allocation.Info));

		if (m_Usage == GPUBufferUsage::Dynamic)
		{
			VK_CHECK_RESULT(vmaMapMemory(VulkanContext::GetInstance().GetMemoryAllocator(), m_Allocation.Handle, &m_Mapped));
		}
	}

	VulkanStagingBuffer VulkanBuffer::FillStagingBuffer(MemorySpan data)
	{
		Grapple_PROFILE_FUNCTION();
		VulkanStagingBuffer stagingBuffer = VulkanContext::GetInstance().GetStagingBufferPool().AllocateStagingBuffer(data.GetSize());

		std::memcpy(stagingBuffer.Mapped, data.GetBuffer(), data.GetSize());

		return stagingBuffer;
	}
}
