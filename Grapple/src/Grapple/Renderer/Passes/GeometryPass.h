#pragma once

#include "Grapple/Renderer/RendererSubmitionQueue.h"
#include "Grapple/Renderer/RendererStatistics.h"

#include "Grapple/Renderer/RenderGraph/RenderGraphPass.h"

#include <vector>

namespace Grapple
{
	class DescriptorSet;
	class DescriptorSetPool;
	class ShaderStorageBuffer;
	class GPUTimer;

	class GeometryPass : public RenderGraphPass 
	{
	public:
		GeometryPass(const RendererSubmitionQueue& opaqueObjects,
			RendererStatistics& statistics,
			Ref<DescriptorSet> primarySet,
			Ref<DescriptorSet> primarySetWithoutShadows,
			Ref<DescriptorSetPool> pool);

		~GeometryPass();

		void OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer) override;
		std::optional<float> GetElapsedTime() const;
	private:
		struct Batch
		{
			Ref<const Mesh> Mesh = nullptr;
			Ref<const Material> Material = nullptr;
			uint32_t SubMesh = 0;
			uint32_t BaseInstance = 0;
			uint32_t InstanceCount = 0;
		};

		struct InstanceData
		{
			glm::vec4 PackedTransform[3];
		};
	
		void CullObjects();

		void FlushBatch(const Ref<CommandBuffer>& commandBuffer, const Batch& batch);
	private:
		Ref<GPUTimer> m_Timer = nullptr;

		Ref<DescriptorSetPool> m_Pool = nullptr;

		RendererStatistics& m_Statistics;
		const RendererSubmitionQueue& m_OpaqueObjects;
		std::vector<uint32_t> m_VisibleObjects;
		std::vector<InstanceData> m_InstanceBuffer;

		Ref<ShaderStorageBuffer> m_InstanceStorageBuffer = nullptr;

		Ref<DescriptorSet> m_PrimaryDescriptorSet = nullptr;
		Ref<DescriptorSet> m_PrimaryDescriptorSetWithoutShadows = nullptr;
	};
}
