#include "DecalsPass.h"

#include "GrappleCore/Profiler/Profiler.h"

#include "Grapple/Renderer/RendererPrimitives.h"
#include "Grapple/Renderer/CommandBuffer.h"
#include "Grapple/Renderer/DescriptorSet.h"
#include "Grapple/Renderer/ShaderStorageBuffer.h"

#include "Grapple/Platform/Vulkan/VulkanCommandBuffer.h"

namespace Grapple
{
	DecalsPass::DecalsPass(const std::vector<DecalSubmitionData>& submitedDecals,
		Ref<DescriptorSet> set,
		Ref<DescriptorSetPool> pool,
		Ref<DescriptorSetPool> decalDescriptorPool,
		Ref<Texture> depthTexture)
		: m_SubmitedDecals(submitedDecals),
		m_PrimarySet(set),
		m_Pool(pool),
		m_DepthTexture(depthTexture),
		m_DecalDescriptorPool(decalDescriptorPool)
	{
		const size_t maxDecals = 1000;
		m_InstanceBuffer = ShaderStorageBuffer::Create(maxDecals * sizeof(InstanceData));

		m_DecalSet = m_DecalDescriptorPool->AllocateSet();

		m_DecalSet->WriteImage(m_DepthTexture, 0);
		m_DecalSet->FlushWrites();

		m_PrimarySet->WriteStorageBuffer(m_InstanceBuffer, 3);
		m_PrimarySet->FlushWrites();
	}

	DecalsPass::~DecalsPass()
	{
		m_DecalDescriptorPool->ReleaseSet(m_DecalSet);
		m_Pool->ReleaseSet(m_PrimarySet);
	}

	void DecalsPass::OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer)
	{
		Grapple_PROFILE_FUNCTION();

		{
			Grapple_PROFILE_SCOPE("FillInstanceData");
			m_InstanceData.clear();
			m_InstanceData.reserve(m_SubmitedDecals.size());

			for (const DecalSubmitionData& decal : m_SubmitedDecals)
			{
				InstanceData& instanceData = m_InstanceData.emplace_back();
				instanceData.PackedTransform[0] = decal.PackedTransform[0];
				instanceData.PackedTransform[1] = decal.PackedTransform[1];
				instanceData.PackedTransform[2] = decal.PackedTransform[2];
			}
		}

		m_InstanceBuffer->SetData(MemorySpan::FromVector(m_InstanceData), 0, commandBuffer);

		commandBuffer->BeginRenderTarget(context.GetRenderTarget());

		Ref<VulkanCommandBuffer> vulkanCommandBuffer = As<VulkanCommandBuffer>(commandBuffer);
		vulkanCommandBuffer->SetPrimaryDescriptorSet(m_PrimarySet);
		vulkanCommandBuffer->SetSecondaryDescriptorSet(m_DecalSet);

		Ref<const Mesh> cubeMesh = RendererPrimitives::GetCube();

		for (size_t decalIndex = 0; decalIndex < m_SubmitedDecals.size(); decalIndex++)
		{
			const DecalSubmitionData& decal = m_SubmitedDecals[decalIndex];

			commandBuffer->ApplyMaterial(decal.Material);
			commandBuffer->DrawMeshIndexed(cubeMesh, 0, (uint32_t)decalIndex, 1);
		}

		commandBuffer->EndRenderTarget();
	}
}
