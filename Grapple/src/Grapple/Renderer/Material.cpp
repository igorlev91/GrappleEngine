#include "Material.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Renderer/Shader.h"
#include "Grapple/Renderer/Texture.h"
#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/Texture.h"

#include "Grapple/Platform/Vulkan/VulkanMaterial.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Grapple
{
	Grapple_IMPL_ASSET(Material);
	Grapple_IMPL_TYPE(Material);

	Material::Material()
		: Asset(AssetType::Material), m_Shader(nullptr) {}

	Material::Material(Ref<Shader> shader)
		: Asset(AssetType::Material), m_Shader(shader)
	{
		Grapple_CORE_ASSERT(shader);
		Initialize();
	}

	Material::Material(AssetHandle shaderHandle)
		: Asset(AssetType::Material), m_Shader(nullptr)
	{
		Grapple_CORE_ASSERT(AssetManager::IsAssetHandleValid(shaderHandle));
		m_Shader = AssetManager::GetAsset<Shader>(shaderHandle);
		
		Initialize();
	}

	Ref<Material> Material::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanMaterial>();
		}

		Grapple_CORE_ASSERT(false);
		return nullptr;
	}

	Ref<Material> Material::Create(Ref<Shader> shader)
	{
		Ref<Material> material = nullptr;
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			material = CreateRef<VulkanMaterial>();
			break;
		}

		Grapple_CORE_ASSERT(material);
		
		material->SetShader(shader);

		return material;
	}

	Ref<Material> Material::Create(AssetHandle shaderHandle)
	{
		Ref<Material> material = nullptr;
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			material = CreateRef<VulkanMaterial>();
			break;
		}

		Grapple_CORE_ASSERT(material);
		
		Grapple_CORE_ASSERT(AssetManager::IsAssetHandleValid(shaderHandle));
		material->SetShader(AssetManager::GetAsset<Shader>(shaderHandle));

		return material;
	}

	void Material::Initialize()
	{
		if (m_Shader == nullptr)
			return;

		const ShaderProperties& properties = m_Shader->GetProperties();
		size_t samplers = 0;

		for (const auto& property : properties)
		{
			if (property.Type == ShaderDataType::Sampler)
				samplers++;
		}

		m_Textures.resize(samplers, nullptr);
		m_ConstantBuffer.SetShader(m_Shader);
	}

	Material::~Material()
	{
	}

	void Material::SetShader(const Ref<Shader>& shader)
	{
		m_Textures.clear();
		m_Shader = shader;
		Initialize();
	}

	const Ref<Texture>& Material::GetTextureProperty(uint32_t propertyIndex) const
	{
		const ShaderProperties& properties = m_Shader->GetProperties();
		Grapple_CORE_ASSERT((size_t)propertyIndex < properties.size());
		Grapple_CORE_ASSERT(properties[propertyIndex].Type == ShaderDataType::Sampler);
		
		return m_Textures[properties[propertyIndex].SamplerIndex];
	}

	void Material::SetTextureProperty(uint32_t propertyIndex, Ref<Texture> texture)
	{
		const ShaderProperties& properties = m_Shader->GetProperties();
		Grapple_CORE_ASSERT((size_t)propertyIndex < properties.size());
		Grapple_CORE_ASSERT(properties[propertyIndex].Type == ShaderDataType::Sampler);
		
		const Ref<Texture>& oldTexture = m_Textures[properties[propertyIndex].SamplerIndex];
		if (oldTexture.get() == texture.get())
			return;

		m_Textures[properties[propertyIndex].SamplerIndex] = texture;
		m_IsDirty = true;
	}
}
