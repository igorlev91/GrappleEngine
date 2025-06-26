#pragma once

#include "Grapple/Renderer/RendererSubmitionQueue.h"
#include "Grapple/Renderer/RenderGraph/RenderGraphPass.h"
#include "Grapple/Renderer/RendererStatistics.h"

#ifndef FIXED_SHADOW_NEAR_AND_FAR
	#define FIXED_SHADOW_NEAR_AND_FAR 1
#endif

namespace Grapple
{
	struct RenderView;
	class DescriptorSet;
	class DescriptorSetPool;
	class UniformBuffer;
	class ShaderStorageBuffer;
	class GPUTimer;
	class ShadowCascadePass : public RenderGraphPass
	{
	public:
		static constexpr size_t MaxCascades = 4;

		ShadowCascadePass(const RendererSubmitionQueue& opaqueObjects,
			RendererStatistics& statistics,
			const RenderView& lightView,
			const std::vector<uint32_t>& visibleObjects,
			Ref<Texture> cascadeTexture);

		~ShadowCascadePass();

		void OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer) override;
	public:
		struct InstanceData
		{
			glm::vec4 PackedTransform[3];
		};

		struct Batch
		{
			Ref<const Mesh> Mesh = nullptr;
			uint32_t SubMesh = 0;
			uint32_t BaseInstance = 0;
			uint32_t InstanceCount = 0;
		};
	private:
		void DrawCascade(const RenderGraphContext& context, const Ref<CommandBuffer>& commandBuffer);
		void FlushBatch(const Ref<CommandBuffer>& commandBuffer, const Batch& batch);
	private:
		const RendererSubmitionQueue& m_OpaqueObjects;
		RendererStatistics& m_Statistics;

		Ref<GPUTimer> m_Timer = nullptr;
		Ref<Texture> m_CascadeTexture = nullptr;

		Ref<UniformBuffer> m_CameraBuffer = nullptr;
		Ref<DescriptorSet> m_CameraDescriptor = nullptr;

		Ref<ShaderStorageBuffer> m_InstanceBuffer = nullptr;
		Ref<DescriptorSet> m_InstanceBufferDescriptor = nullptr;

		const RenderView& m_LightView;
		const std::vector<uint32_t>& m_VisibleObjects;

		std::vector<InstanceData> m_InstanceDataBuffer;
	};
}
