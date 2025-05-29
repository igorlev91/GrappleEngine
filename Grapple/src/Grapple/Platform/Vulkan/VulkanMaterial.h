#pragma once

#include "Grapple/Renderer/Material.h"
#include "Grapple/Renderer/Pipeline.h"

namespace Grapple
{
	class VulkanPipeline;
	class VulkanDescriptorSet;
	class VulkanRenderPass;

	class VulkanMaterial : public Material
	{
	public:
		virtual void SetShader(const Ref<Shader>& shader) override;

		Ref<VulkanPipeline> GetPipeline(const Ref<VulkanRenderPass>& renderPass);

		Span<Ref<VulkanDescriptorSet>> GetSets() const { return Span((Ref<VulkanDescriptorSet>*)&m_UsedSets, 2); }
	private:
		Ref<VulkanPipeline> m_Pipeline = nullptr;
		Ref<VulkanDescriptorSet> m_UsedSets[4] = { nullptr };
	};
}
