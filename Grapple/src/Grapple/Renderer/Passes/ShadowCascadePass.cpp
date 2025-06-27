#include "ShadowCascadePass.h"

#include "Grapple/Renderer/Viewport.h"
#include "Grapple/Renderer/RenderData.h"
#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/UniformBuffer.h"
#include "Grapple/Renderer/ShaderStorageBuffer.h"
#include "Grapple/Renderer/CommandBuffer.h"
#include "Grapple/Renderer/GPUTimer.h"

#include "Grapple/Renderer/Passes/ShadowPass.h"

#include "Grapple/Math/Math.h"
#include "Grapple/Math/SIMD.h"

#include "Grapple/Platform/Vulkan/VulkanContext.h"
#include "Grapple/Platform/Vulkan/VulkanCommandBuffer.h"

namespace Grapple
{
	ShadowCascadePass::ShadowCascadePass(const RendererSubmitionQueue& opaqueObjects,
		RendererStatistics& statistics,
		const ShadowCascadeData& cascadeData,
		const std::vector<Math::Compact3DTransform>& filteredTransforms,
		Ref<Texture> cascadeTexture)
		: m_OpaqueObjects(opaqueObjects),
		m_Statistics(statistics),
		m_CascadeData(cascadeData),
		m_FilteredTransforms(filteredTransforms),
		m_CascadeTexture(cascadeTexture)
	{
		Grapple_PROFILE_FUNCTION();

		m_Timer = GPUTimer::Create();

		m_CameraBuffer = UniformBuffer::Create(sizeof(RenderView));
		m_CameraDescriptor = Renderer::GetCameraDescriptorSetPool()->AllocateSet();
		m_CameraDescriptor->WriteUniformBuffer(m_CameraBuffer, 0);
		m_CameraDescriptor->FlushWrites();

		constexpr size_t maxInstanceCount = 1000;
		m_InstanceBuffer = ShaderStorageBuffer::Create(maxInstanceCount * sizeof(InstanceData));
		m_InstanceBufferDescriptor = Renderer::GetInstanceDataDescriptorSetPool()->AllocateSet();
		m_InstanceBufferDescriptor->WriteStorageBuffer(m_InstanceBuffer, 0);
		m_InstanceBufferDescriptor->FlushWrites();
	}

	ShadowCascadePass::~ShadowCascadePass()
	{
		Renderer::GetInstanceDataDescriptorSetPool()->ReleaseSet(m_InstanceBufferDescriptor);
		Renderer::GetCameraDescriptorSetPool()->ReleaseSet(m_CameraDescriptor);
	}

	void ShadowCascadePass::OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer)
	{
		Grapple_PROFILE_FUNCTION();

		const Viewport& currentViewport = context.GetViewport();
		const ShadowSettings& shadowSettings = Renderer::GetShadowSettings();

		if (m_CascadeData.Batches.size() == 0)
		{
			commandBuffer->BeginRenderTarget(context.GetRenderTarget());
			commandBuffer->EndRenderTarget();
			return;
		}

		m_CameraBuffer->SetData(&m_CascadeData.View, sizeof(m_CascadeData.View), 0);

		m_Statistics.ShadowPassTime += m_Timer->GetElapsedTime().value_or(0.0f);

		{
			Grapple_PROFILE_SCOPE("FillInstanceData");

			m_InstanceDataBuffer.clear();

			for (const auto& batch : m_CascadeData.Batches)
			{
				for (uint32_t i = 0; i < batch.Count; i++)
				{
					auto& instanceData = m_InstanceDataBuffer.emplace_back();
					const auto& transform = m_FilteredTransforms[batch.FirstEntryIndex + i];
					instanceData.PackedTransform[0] = glm::vec4(transform.RotationScale[0], transform.Translation.x);
					instanceData.PackedTransform[1] = glm::vec4(transform.RotationScale[1], transform.Translation.y);
					instanceData.PackedTransform[2] = glm::vec4(transform.RotationScale[2], transform.Translation.z);
				}
			}
		}

		m_InstanceBuffer->SetData(MemorySpan::FromVector(m_InstanceDataBuffer), 0, commandBuffer);

		commandBuffer->StartTimer(m_Timer);

		commandBuffer->BeginRenderTarget(context.GetRenderTarget());
		uint32_t shadowMapResolution = GetShadowMapResolution(shadowSettings.Quality);
		commandBuffer->SetViewportAndScisors(Math::Rect(0.0f, 0.0f, (float)shadowMapResolution, (float)shadowMapResolution));

		DrawCascade(context, commandBuffer);

		commandBuffer->EndRenderTarget();

		commandBuffer->StopTimer(m_Timer);
	}

	void ShadowCascadePass::DrawCascade(const RenderGraphContext& context, const Ref<CommandBuffer>& commandBuffer)
	{
		Grapple_PROFILE_FUNCTION();

		commandBuffer->SetGlobalDescriptorSet(m_CameraDescriptor, 0);
		commandBuffer->SetGlobalDescriptorSet(context.GetViewport().GlobalResources.GlobalDescriptorSetWithoutShadows, 1);
		commandBuffer->SetGlobalDescriptorSet(m_InstanceBufferDescriptor, 2);

		commandBuffer->ApplyMaterial(Renderer::GetDepthOnlyMaterial());

		uint32_t instanceIndex = 0;

		for (const auto& batch : m_CascadeData.Batches)
		{
			commandBuffer->DrawMeshIndexed(batch.Mesh, instanceIndex, batch.Count);
			instanceIndex += batch.Count;
		}
	}

	void ShadowCascadePass::FlushBatch(const Ref<CommandBuffer>& commandBuffer, const Batch& batch)
	{
		Grapple_PROFILE_FUNCTION();

		if (batch.InstanceCount == 0)
			return;

		m_Statistics.DrawCallCount++;
		m_Statistics.DrawCallsSavedByInstancing += batch.InstanceCount - 1;

		commandBuffer->DrawMeshIndexed(batch.Mesh, batch.SubMesh, batch.BaseInstance, batch.InstanceCount);
	}
}
