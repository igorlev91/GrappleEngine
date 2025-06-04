#include "GeometryPass.h"

#include "Grapple/Renderer/GraphicsContext.h"
#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/Viewport.h"
#include "Grapple/Renderer/ShaderStorageBuffer.h"
#include "Grapple/Renderer/GPUTimer.h"

#include "Grapple/Math/SIMD.h"

#include "Grapple/Platform/Vulkan/VulkanCommandBuffer.h"
#include "Grapple/Platform/Vulkan/VulkanContext.h"

namespace Grapple
{
	GeometryPass::GeometryPass(const RendererSubmitionQueue& opaqueObjects,
		Ref<DescriptorSet> primarySet,
		Ref<DescriptorSet> primarySetWithoutShadows)
		: RenderPass(RenderPassQueue::BeforeOpaqueGeometry),
		m_OpaqueObjects(opaqueObjects),
		m_PrimaryDescriptorSet(primarySet),
		m_PrimaryDescriptorSetWithoutShadows(primarySetWithoutShadows)
	{
		constexpr size_t maxInstances = 1000;
		m_InstanceStorageBuffer = ShaderStorageBuffer::Create(maxInstances * sizeof(InstanceData), 0);

		// HACK
		primarySet->WriteStorageBuffer(m_InstanceStorageBuffer, 3);
		primarySet->FlushWrites();

		primarySetWithoutShadows->WriteStorageBuffer(m_InstanceStorageBuffer, 3);
		primarySetWithoutShadows->FlushWrites();

		m_Timer = GPUTimer::Create();
	}

	void GeometryPass::OnRender(RenderingContext& context)
	{
		Grapple_PROFILE_FUNCTION();
		const Viewport& currentViewport = Renderer::GetCurrentViewport();

		Ref<CommandBuffer> commandBuffer = GraphicsContext::GetInstance().GetCommandBuffer();

		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{
			Ref<VulkanCommandBuffer> commandBuffer = VulkanContext::GetInstance().GetPrimaryCommandBuffer();
			Ref<VulkanFrameBuffer> renderTarget = As<VulkanFrameBuffer>(context.RenderTarget);
			
			if (Renderer::GetShadowSettings().Enabled)
			{
				commandBuffer->SetPrimaryDescriptorSet(m_PrimaryDescriptorSet);
			}
			else
			{
				commandBuffer->SetPrimaryDescriptorSet(m_PrimaryDescriptorSetWithoutShadows);
			}

			commandBuffer->SetSecondaryDescriptorSet(nullptr);
		}

		m_VisibleObjects.clear();

		CullObjects();

		// TODO: stats
		// s_RendererData.Statistics.ObjectsCulled += (uint32_t)(s_RendererData.OpaqueQueue.GetSize() - s_RendererData.CulledObjectIndices.size());

		{
			Grapple_PROFILE_SCOPE("Sort");
			std::sort(m_VisibleObjects.begin(), m_VisibleObjects.end(), [this](uint32_t a, uint32_t b) -> bool
			{
				return m_OpaqueObjects[a].SortKey < m_OpaqueObjects[b].SortKey;
			});
		}

		m_InstanceBuffer.clear();

		{
			Grapple_PROFILE_SCOPE("FillInstacesData");
			for (uint32_t objectIndex : m_VisibleObjects)
			{
				auto& instanceData = m_InstanceBuffer.emplace_back();
				const auto& transform = m_OpaqueObjects[objectIndex].Transform;
				instanceData.PackedTransform[0] = glm::vec4(transform.RotationScale[0], transform.Translation.x);
				instanceData.PackedTransform[1] = glm::vec4(transform.RotationScale[1], transform.Translation.y);
				instanceData.PackedTransform[2] = glm::vec4(transform.RotationScale[2], transform.Translation.z);
			}
		}

		{
			Grapple_PROFILE_SCOPE("SetIntancesData");
			m_InstanceStorageBuffer->SetData(MemorySpan::FromVector(m_InstanceBuffer), 0, commandBuffer);
		}

		commandBuffer->StartTimer(m_Timer);
		commandBuffer->BeginRenderTarget(context.RenderTarget);
		commandBuffer->SetViewportAndScisors(Math::Rect(glm::vec2(0.0f, 0.0f), (glm::vec2)context.RenderTarget->GetSize()));

		Batch batch{};

		for (uint32_t currentInstance = 0; currentInstance < (uint32_t)m_VisibleObjects.size(); currentInstance++)
		{
			uint32_t objectIndex = m_VisibleObjects[currentInstance];
			const auto& object = m_OpaqueObjects[objectIndex];

			if (batch.Mesh.get() != object.Mesh.get()
				|| batch.SubMesh != object.SubMeshIndex)
			{
				batch.InstanceCount = currentInstance - batch.BaseInstance;

				FlushBatch(commandBuffer, batch);

				batch.BaseInstance = currentInstance;
				batch.InstanceCount = 0;
				batch.Mesh = object.Mesh;
				batch.SubMesh = object.SubMeshIndex;
			}

			if (object.Material.get() != batch.Material.get())
			{
				batch.InstanceCount = currentInstance - batch.BaseInstance;

				FlushBatch(commandBuffer, batch);
				batch.BaseInstance = currentInstance;
				batch.InstanceCount = 0;
				batch.Material = object.Material;
			}
		}

		batch.InstanceCount = (uint32_t)m_VisibleObjects.size() - batch.BaseInstance;
		FlushBatch(commandBuffer, batch);

		commandBuffer->EndRenderTarget();
		commandBuffer->StopTimer(m_Timer);
	}

	void GeometryPass::CullObjects()
	{
		Grapple_PROFILE_FUNCTION();

		Math::AABB objectAABB;

		const FrustumPlanes& planes = Renderer::GetCurrentViewport().FrameData.CameraFrustumPlanes;
		for (size_t i = 0; i < m_OpaqueObjects.GetSize(); i++)
		{
			const auto& object = m_OpaqueObjects[i];
			objectAABB = Math::SIMD::TransformAABB(object.Mesh->GetSubMeshes()[object.SubMeshIndex].Bounds, object.Transform.ToMatrix4x4());

			bool intersects = true;
			for (size_t i = 0; i < planes.PlanesCount; i++)
			{
				if (!objectAABB.IntersectsOrInFrontOfPlane(planes.Planes[i]))
				{
					intersects = false;
					break;
				}
			}

			if (intersects)
			{
				m_VisibleObjects.push_back((uint32_t)i);
			}
		}
	}

	void GeometryPass::FlushBatch(const Ref<CommandBuffer>& commandBuffer, const Batch& batch)
	{
		Grapple_PROFILE_FUNCTION();

		if (batch.InstanceCount == 0)
			return;

		commandBuffer->ApplyMaterial(batch.Material);
		commandBuffer->DrawIndexed(batch.Mesh, batch.SubMesh, batch.BaseInstance, batch.InstanceCount);
	}
}
