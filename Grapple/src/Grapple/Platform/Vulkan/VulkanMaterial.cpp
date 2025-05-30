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
		specifications.Shader = m_Shader;

		if (m_Shader->GetMetadata()->Type == ShaderType::_2D)
		{
			specifications.DepthTest = false;
			specifications.DepthWrite = false;
			specifications.InputLayout = PipelineInputLayout({
				{ 0, 0, ShaderDataType::Float3 }, // Position
				{ 0, 1, ShaderDataType::Float4 }, // Color
				{ 0, 2, ShaderDataType::Float2 }, // UV
				{ 0, 3, ShaderDataType::Float }, // Texture index
				{ 0, 4, ShaderDataType::Int }, // Entity index
			});
		}
		else if (m_Shader->GetMetadata()->Type == ShaderType::Surface)
		{
			specifications.DepthTest = true;
			specifications.DepthWrite = true;
			specifications.InputLayout = PipelineInputLayout({
				{ 0, 0, ShaderDataType::Float3 }, // Position
				{ 1, 1, ShaderDataType::Float3 }, // Normal
				{ 2, 2, ShaderDataType::Float3 }, // Tangent
				{ 3, 3, ShaderDataType::Float2 }, // UV
			});
		}
		else
		{
			Grapple_CORE_ASSERT(false);
		}
		
		m_Pipeline = CreateRef<VulkanPipeline>(specifications, renderPass);
		return m_Pipeline;
	}
}
