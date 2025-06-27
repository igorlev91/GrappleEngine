#pragma once

#include "Grapple/Renderer/RendererSubmitionQueue.h"
#include "Grapple/Renderer/RenderGraph/RenderGraphPass.h"
#include "Grapple/Renderer/RendererStatistics.h"

namespace Grapple
{
	struct RenderView;
	class DescriptorSet;
	class DescriptorSetPool;
	class UniformBuffer;
	class ShaderStorageBuffer;
	class GPUTimer;

	struct ShadowCascadeData;
	struct VisibleSubMeshRange;

	class ShadowCascadePass : public RenderGraphPass
	{
	public:
		static constexpr size_t MaxCascades = 4;

		ShadowCascadePass(const RendererSubmitionQueue& opaqueObjects,
			RendererStatistics& statistics,
			const ShadowCascadeData& cascadeData,
			const std::vector<Math::Compact3DTransform>& filteredTransforms,
			const std::vector<VisibleSubMeshRange>& visibleSubMeshRanges);

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
	private:
		const RendererSubmitionQueue& m_OpaqueObjects;
		RendererStatistics& m_Statistics;

		const ShadowCascadeData& m_CascadeData;
		const std::vector<Math::Compact3DTransform>& m_FilteredTransforms;
		const std::vector<VisibleSubMeshRange>& m_VisibleSubMeshRanges;

		Ref<GPUTimer> m_Timer = nullptr;

		Ref<UniformBuffer> m_CameraBuffer = nullptr;
		Ref<DescriptorSet> m_CameraDescriptor = nullptr;

		Ref<ShaderStorageBuffer> m_InstanceBuffer = nullptr;
		Ref<DescriptorSet> m_InstanceBufferDescriptor = nullptr;

		std::vector<InstanceData> m_InstanceDataBuffer;
	};
}
