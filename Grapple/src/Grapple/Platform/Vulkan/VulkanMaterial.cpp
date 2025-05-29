#include "VulkanMaterial.h"

#include "Grapple/Platform/Vulkan/VulkanPipeline.h"
#include "Grapple/Platform/Vulkan/VulkanShader.h"
#include "Grapple/Platform/Vulkan/VulkanDescriptorSet.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer2D/Renderer2D.h"

namespace Grapple
{
	void VulkanMaterial::SetShader(const Ref<Shader>& shader)
	{
		Material::SetShader(shader);
	}

	Ref<VulkanPipeline> VulkanMaterial::GetPipeline(const Ref<VulkanRenderPass>& renderPass)
	{
		if (m_Pipeline != nullptr && m_Pipeline->GetCompatibleRenderPass().get() == renderPass.get())
			return m_Pipeline;

		PipelineSpecifications specifications{};
		specifications.Culling = CullingMode::Back;
		specifications.DepthFunction = DepthComparisonFunction::Less;
		specifications.DepthTest = false;
		specifications.DepthWrite = true;
		specifications.Shader = m_Shader;

		if (m_Shader->GetMetadata()->Type == ShaderType::_2D)
		{
			specifications.InputLayout = BufferLayout({
				BufferLayoutElement("i_Position", ShaderDataType::Float3),
				BufferLayoutElement("i_Color", ShaderDataType::Float4),
				BufferLayoutElement("i_UV", ShaderDataType::Float2),
				BufferLayoutElement("i_TextureIndex", ShaderDataType::Float),
				BufferLayoutElement("i_EntityIndex", ShaderDataType::Int),
			});

			m_UsedSets[0] = Renderer::GetPrimaryDescriptorSet();
			m_UsedSets[1] = Renderer2D::GetDescriptorSet();
		}
		else
		{
			Grapple_CORE_ASSERT(false);
		}
		
		m_Pipeline = CreateRef<VulkanPipeline>(specifications, renderPass);
		return m_Pipeline;
	}
}
