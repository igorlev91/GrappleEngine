#include "GeometryPass.h"

#include "Grapple/Renderer/GraphicsContext.h"
#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/Viewport.h"
#include "Grapple/Renderer/ShaderStorageBuffer.h"
#include "Grapple/Renderer/GPUTimer.h"

#include "Grapple/Renderer2D/Renderer2D.h"

#include "Grapple/Math/SIMD.h"

#include "Grapple/Platform/Vulkan/VulkanCommandBuffer.h"
#include "Grapple/Platform/Vulkan/VulkanContext.h"

namespace Grapple
{
	GeometryPass::GeometryPass(const RendererSubmitionQueue& opaqueObjects,
		RendererStatistics& statistics,
		Ref<DescriptorSet> primarySet,
		Ref<DescriptorSet> primarySetWithoutShadows,
		Ref<DescriptorSetPool> pool)
		: m_OpaqueObjects(opaqueObjects),
		m_Statistics(statistics),
		m_PrimaryDescriptorSet(primarySet),
		m_PrimaryDescriptorSetWithoutShadows(primarySetWithoutShadows),
		m_Pool(pool)
	{
		Grapple_PROFILE_FUNCTION();
		constexpr size_t maxInstances = 1000;
		m_InstanceStorageBuffer = ShaderStorageBuffer::Create(maxInstances * sizeof(InstanceData));

		// HACK
		primarySet->WriteStorageBuffer(m_InstanceStorageBuffer, 3);
		primarySet->FlushWrites();

		primarySetWithoutShadows->WriteStorageBuffer(m_InstanceStorageBuffer, 3);
		primarySetWithoutShadows->FlushWrites();

		m_Timer = GPUTimer::Create();
	}

	GeometryPass::~GeometryPass()
	{
		m_Pool->ReleaseSet(m_PrimaryDescriptorSet);
		m_Pool->ReleaseSet(m_PrimaryDescriptorSetWithoutShadows);
	}

	void GeometryPass::OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer)
	{
		Grapple_PROFILE_FUNCTION();

		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{
			Ref<VulkanCommandBuffer> vulkanCommandBuffer = As<VulkanCommandBuffer>(commandBuffer);
			
			if (Renderer::GetShadowSettings().Enabled)
			{
				vulkanCommandBuffer->SetPrimaryDescriptorSet(m_PrimaryDescriptorSet);
			}
			else
			{
				vulkanCommandBuffer->SetPrimaryDescriptorSet(m_PrimaryDescriptorSetWithoutShadows);
			}

			vulkanCommandBuffer->SetSecondaryDescriptorSet(nullptr);
		}

		m_VisibleObjects.clear();

		CullObjects();

		m_Statistics.GeometryPassTime += m_Timer->GetElapsedTime().value_or(0.0f); // Read the time from the previous frame
		m_Statistics.ObjectsVisible += (uint32_t)m_VisibleObjects.size();

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

		m_InstanceStorageBuffer->SetData(MemorySpan::FromVector(m_InstanceBuffer), 0, commandBuffer);

		Ref<FrameBuffer> renderTarget = context.GetRenderTarget();

		commandBuffer->StartTimer(m_Timer);
		commandBuffer->BeginRenderTarget(renderTarget);
		commandBuffer->SetViewportAndScisors(Math::Rect(glm::vec2(0.0f, 0.0f), (glm::vec2)renderTarget->GetSize()));

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

	std::optional<float> GeometryPass::GetElapsedTime() const
	{
		return m_Timer->GetElapsedTime();
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

		m_Statistics.DrawCallCount++;
		m_Statistics.DrawCallsSavedByInstancing += batch.InstanceCount - 1;

		commandBuffer->ApplyMaterial(batch.Material);
		commandBuffer->DrawIndexed(batch.Mesh, batch.SubMesh, batch.BaseInstance, batch.InstanceCount);
	}
}
