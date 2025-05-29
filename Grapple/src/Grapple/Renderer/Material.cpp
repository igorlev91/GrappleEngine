#include "Material.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Renderer/Shader.h"
#include "Grapple/Renderer/Texture.h"
#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Renderer/Renderer.h"

#include "Grapple/Platform/Vulkan/VulkanMaterial.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Grapple
{
	void TexturePropertyValue::SetTexture(const Ref<Grapple::Texture>& texture)
	{
		ValueType = Type::Texture;
		Texture = texture;
		FrameBuffer = nullptr;
		FrameBufferAttachmentIndex = UINT32_MAX;
	}

	void TexturePropertyValue::SetFrameBuffer(const Ref<Grapple::FrameBuffer>& frameBuffer, uint32_t attachment)
	{
		Grapple_CORE_ASSERT(frameBuffer);
		Grapple_CORE_ASSERT(attachment < frameBuffer->GetAttachmentsCount());

		ValueType = Type::FrameBufferAttachment;
		Texture = nullptr;
		FrameBuffer = frameBuffer;
		FrameBufferAttachmentIndex = attachment;
	}

	void TexturePropertyValue::Clear()
	{
		ValueType = Type::Texture;
		Texture = nullptr;
		FrameBuffer = nullptr;
		FrameBufferAttachmentIndex = UINT32_MAX;
	}

	Grapple_IMPL_ASSET(Material);
	Grapple_IMPL_TYPE(Material);

	Material::Material()
		: Asset(AssetType::Material), m_Shader(nullptr), m_Buffer(nullptr), m_BufferSize(0) {}

	Material::Material(Ref<Shader> shader)
		: Asset(AssetType::Material),
		m_Shader(shader),
		m_Buffer(nullptr),
		m_BufferSize(0)
	{
		Grapple_CORE_ASSERT(shader);
		Initialize();
	}

	Material::Material(AssetHandle shaderHandle)
		: Asset(AssetType::Material),
		m_Shader(nullptr),
		m_Buffer(nullptr),
		m_BufferSize(0)
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
		case RendererAPI::API::OpenGL:
			return CreateRef<Material>();
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
		case RendererAPI::API::OpenGL:
			material = CreateRef<Material>();
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
		case RendererAPI::API::OpenGL:
			material = CreateRef<Material>();
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

		for (const auto& prop : properties)
		{
			m_BufferSize += prop.Size;

			if (prop.Type == ShaderDataType::Sampler)
				samplers++;
		}

		m_Textures.resize(samplers, TexturePropertyValue());

		if (properties.size() > 0)
			m_BufferSize += properties.back().Size;

		if (m_BufferSize != 0)
		{
			m_Buffer = new uint8_t[m_BufferSize];
			std::memset(m_Buffer, 0, m_BufferSize);
		}
	}

	Material::~Material()
	{
		if (m_Buffer != nullptr)
			delete[] m_Buffer;
	}

	void Material::SetShader(const Ref<Shader>& shader)
	{
		if (m_Buffer != nullptr)
		{
			delete[] m_Buffer;
			m_Buffer = nullptr;
			m_BufferSize = 0;
		}

		m_Textures.clear();
		m_Shader = shader;
		Initialize();
	}

	void Material::SetIntArray(uint32_t index, const int32_t* values, uint32_t count)
	{
		const ShaderProperties& properties = m_Shader->GetProperties();
		Grapple_CORE_ASSERT((size_t)index < properties.size());
		memcpy_s(m_Buffer + properties[index].Offset, properties[index].Size, values, sizeof(*values) * count);
	}

	template<>
	Grapple_API TexturePropertyValue& Material::GetPropertyValue(uint32_t index)
	{
		const ShaderProperties& properties = m_Shader->GetProperties();
		Grapple_CORE_ASSERT((size_t)index < properties.size());
		Grapple_CORE_ASSERT(properties[index].Type == ShaderDataType::Sampler);
		Grapple_CORE_ASSERT(properties[index].SamplerIndex < (uint32_t)m_Textures.size());

		return m_Textures[properties[index].SamplerIndex];
	}

	template<>
	Grapple_API TexturePropertyValue Material::ReadPropertyValue(uint32_t index) const
	{
		const ShaderProperties& properties = m_Shader->GetProperties();
		Grapple_CORE_ASSERT((size_t)index < properties.size());
		Grapple_CORE_ASSERT(properties[index].Type == ShaderDataType::Sampler);
		Grapple_CORE_ASSERT(properties[index].SamplerIndex < (uint32_t)m_Textures.size());

		return m_Textures[properties[index].SamplerIndex];
	}

	template<>
	Grapple_API void Material::WritePropertyValue(uint32_t index, const TexturePropertyValue& value)
	{
		const ShaderProperties& properties = m_Shader->GetProperties();
		Grapple_CORE_ASSERT((size_t)index < properties.size());
		Grapple_CORE_ASSERT(properties[index].Type == ShaderDataType::Sampler);
		Grapple_CORE_ASSERT(properties[index].SamplerIndex < (uint32_t)m_Textures.size());

		m_Textures[properties[index].SamplerIndex] = value;
	}
}
