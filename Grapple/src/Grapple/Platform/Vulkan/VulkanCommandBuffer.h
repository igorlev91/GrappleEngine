#pragma once

#include "Grapple/Renderer/Pipeline.h"
#include "Grapple/Renderer/CommandBuffer.h"

#include "Grapple/Platform/Vulkan/VulkanRenderPass.h"
#include "Grapple/Platform/Vulkan/VulkanFrameBuffer.h"
#include "Grapple/Platform/Vulkan/VulkanDescriptorSet.h"

#include <vulkan/vulkan.h>

#include <unordered_set>

namespace Grapple
{
	class Material;
	class VulkanPipeline;
	class VulkanRenderPass;
	class VulkanGPUTimer;

	class Grapple_API VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer(VkCommandBuffer commandBuffer);

		void BeginRenderTarget(const Ref<FrameBuffer> frameBuffer) override;
		void EndRenderTarget() override;

		void ClearColorAttachment(Ref<FrameBuffer> frameBuffer, uint32_t index, const glm::vec4& clearColor) override;
		void ClearDepthAttachment(Ref<FrameBuffer> frameBuffer, float depth) override;

		void ClearColor(const Ref<Texture>& texture, const glm::vec4& clearColor) override;
		void ClearDepth(const Ref<Texture>& texture, float depth) override;

		void ApplyMaterial(const Ref<const Material>& material) override;

		void SetViewportAndScisors(Math::Rect viewportRect) override;

		void DrawIndexed(const Ref<const Mesh>& mesh, uint32_t subMeshIndex, uint32_t baseInstance, uint32_t instanceCount) override;

		// Expects source attachment to be in TRANSFER_SRC
		// Destination attachment - TRANSFER_DST
		// Leaves layouts unchanged
		void Blit(Ref<FrameBuffer> source, uint32_t sourceAttachment, Ref<FrameBuffer> destination, uint32_t destinationAttachment, TextureFiltering filter) override;
		void Blit(Ref<Texture> source, Ref<Texture> destination, TextureFiltering filter) override;

		void DispatchCompute(Ref<ComputePipeline> pipeline, const glm::uvec3& groupCount) override;

		void StartTimer(Ref<GPUTimer> timer) override;
		void StopTimer(Ref<GPUTimer> timer) override;
	public:
		void Reset();

		void Begin();
		void End();

		void BeginRenderPass(const Ref<VulkanRenderPass>& renderPass, const Ref<VulkanFrameBuffer>& frameBuffer);
		void EndRenderPass();

		void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
		void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t baseMipLevel, uint32_t mipLevels);
		void TransitionDepthImageLayout(VkImage image, bool hasStencilComponent, VkImageLayout oldLayout, VkImageLayout newLayout);

		void ClearImage(VkImage image, const glm::vec4& clearColor, VkImageLayout oldLayout, VkImageLayout newLayout);
		void ClearDepthStencilImage(VkImage image, bool hasStencilComponent, float depthValue, uint32_t stencilValue, VkImageLayout oldLayout, VkImageLayout newLayout);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, VkExtent3D size, size_t bufferOffset, uint32_t mip);
		void CopyBuffer(VkBuffer sourceBuffer, VkBuffer destinationBuffer, size_t size, size_t sourceOffset, size_t destinationOffset);

		void AddBufferBarrier(Span<VkBufferMemoryBarrier> memoryBarriers, VkPipelineStageFlags sourceStages, VkPipelineStageFlags destinationStages);

		void GenerateImageMipMaps(VkImage image, uint32_t mipLevels, glm::uvec2 imageSize);

		void BindPipeline(const Ref<const Pipeline>& pipeline);
		void BindVertexBuffers(const Span<Ref<const VertexBuffer>>& vertexBuffers);
		void BindIndexBuffer(const Ref<const IndexBuffer>& indexBuffer);

		void BindDescriptorSet(const Ref<const VulkanDescriptorSet>& descriptorSet, VkPipelineLayout pipelineLayout, uint32_t index);
		void BindComputeDescriptorSet(const Ref<const VulkanDescriptorSet>& descriptorSet, VkPipelineLayout pipelineLayout, uint32_t index);

		void SetPrimaryDescriptorSet(const Ref<DescriptorSet>& set);
		void SetSecondaryDescriptorSet(const Ref<DescriptorSet>& set);

		void DrawIndexed(uint32_t indicesCount);
		void DrawIndexed(uint32_t firstIndex, uint32_t indicesCount);
		void DrawIndexed(uint32_t firstIndex, uint32_t indicesCount, uint32_t firstInstance, uint32_t instancesCount);

		void Draw(uint32_t firstVertex, uint32_t vertexCount, uint32_t firstInstance, uint32_t instanceCount);

		void DepthImagesBarrier(Span<VkImage> images, bool hasStencil,
			VkPipelineStageFlags srcStage, VkAccessFlags srcAccessMask,
			VkPipelineStageFlags dstStage, VkAccessFlags dstAccessMask,
			VkImageLayout oldLayout, VkImageLayout newLayout);

		void BeginTimer(Ref<VulkanGPUTimer> timer, VkPipelineStageFlagBits pipelineStages);
		void EndTimer(Ref<VulkanGPUTimer> timer, VkPipelineStageFlagBits pipelineStages);

		VkCommandBuffer GetHandle() const { return m_CommandBuffer; }
	private:
		std::vector<VkImageMemoryBarrier> m_ImageBarriers;

		Ref<const Mesh> m_CurrentMesh = nullptr;

		Ref<VulkanDescriptorSet> m_PrimaryDescriptorSet = nullptr;
		Ref<VulkanDescriptorSet> m_SecondaryDescriptorSet = nullptr;

		struct BoundDescriptorSet
		{
			Ref<const VulkanDescriptorSet> Set = nullptr;
			VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
		};

		BoundDescriptorSet m_CurrentDescriptorSets[4] = { nullptr };
		Ref<const Pipeline> m_CurrentGraphicsPipeline = nullptr;

		VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
		Ref<VulkanRenderPass> m_CurrentRenderPass = nullptr;

		std::vector<Ref<const Pipeline>> m_UsedPipelines;
	};
}
