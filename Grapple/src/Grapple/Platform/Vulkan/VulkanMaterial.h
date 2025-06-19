#pragma once

#include "Grapple/Renderer/Material.h"
#include "Grapple/Renderer/Pipeline.h"

namespace Grapple
{
	class DescriptorSet;

	class VulkanPipeline;
	class VulkanDescriptorSet;
	class VulkanRenderPass;

	class Grapple_API VulkanMaterial : public Material
	{
	public:
		virtual ~VulkanMaterial();

		virtual void SetShader(const Ref<Shader>& shader) override;

		Ref<VulkanPipeline> GetPipeline(const Ref<VulkanRenderPass>& renderPass);
		Ref<DescriptorSet> GetDescriptorSet() const { return m_Set; }

		void UpdateDescriptorSet();
	private:
		void ReleaseDescriptorSet();
	private:
		Ref<VulkanPipeline> m_Pipeline = nullptr;
		Ref<DescriptorSet> m_Set = nullptr;
	};
}
