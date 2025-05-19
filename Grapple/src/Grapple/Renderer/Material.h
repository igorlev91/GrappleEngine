#pragma once

#include "GrappleCore/Serialization/TypeInitializer.h"

#include "Grapple/AssetManager/Asset.h"
#include "Grapple/Renderer/Shader.h"
#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Renderer/ShaderMetadata.h"
#include "Grapple/Renderer/FrameBuffer.h"

namespace Grapple
{
	struct Grapple_API TexturePropertyValue
	{
		enum class Type : uint8_t
		{
			Texture,
			FrameBufferAttachment,
		};

		TexturePropertyValue()
		{
			Clear();
		}

		void SetTexture(const Ref<Texture>& texture);
		void SetFrameBuffer(const Ref<FrameBuffer>& frameBuffer, uint32_t attachment);
		void Clear();

		Type ValueType = Type::Texture;

		Ref<Texture> Texture = nullptr;
		Ref<FrameBuffer> FrameBuffer = nullptr;
		uint32_t FrameBufferAttachmentIndex = UINT32_MAX;
	};

	class Grapple_API Material : public Asset
	{
	public:
		Grapple_ASSET;
		Grapple_TYPE;

		Material();
		Material(Ref<Shader> shader);
		Material(AssetHandle shaderHandle);
		virtual ~Material();

		inline Ref<Shader> GetShader() const { return m_Shader; }
		void SetShader(const Ref<Shader>& shader);

		void SetIntArray(uint32_t index, const int32_t* values, uint32_t count);

		template<typename T>
		T& GetPropertyValue(uint32_t index)
		{
			const ShaderProperties& properties = m_Shader->GetProperties();
			Grapple_CORE_ASSERT((size_t)index < properties.size());
			Grapple_CORE_ASSERT(sizeof(T) == properties[index].Size);
			Grapple_CORE_ASSERT(properties[index].Type != ShaderDataType::Sampler);
			Grapple_CORE_ASSERT(properties[index].Offset + properties[index].Size <= m_BufferSize);
			return *(T*)(m_Buffer + properties[index].Offset);
		}

		template<typename T>
		T ReadPropertyValue(uint32_t index)
		{
			const ShaderProperties& properties = m_Shader->GetProperties();
			Grapple_CORE_ASSERT((size_t)index < properties.size());
			Grapple_CORE_ASSERT(sizeof(T) == properties[index].Size);

			Grapple_CORE_ASSERT(properties[index].Type != ShaderDataType::Sampler);
			Grapple_CORE_ASSERT(properties[index].Offset + properties[index].Size <= m_BufferSize);

			T value;

			memcpy_s(&value, sizeof(value), m_Buffer + properties[index].Offset, properties[index].Size);
			return value;
		}

		template<typename T>
		void WritePropertyValue(uint32_t index, const T& value)
		{
			const ShaderProperties& properties = m_Shader->GetProperties();
			Grapple_CORE_ASSERT((size_t)index < properties.size());
			Grapple_CORE_ASSERT(sizeof(T) == properties[index].Size);
			Grapple_CORE_ASSERT(properties[index].Offset + properties[index].Size <= m_BufferSize);

			Grapple_CORE_ASSERT(properties[index].Type != ShaderDataType::Sampler);
			memcpy_s(m_Buffer + properties[index].Offset, sizeof(value), &value, properties[index].Size);
		}

		inline uint8_t* GetPropertiesBuffer() { return m_Buffer; }

		void SetShaderProperties();
	private:
		void Initialize();
	private:
		Ref<Shader> m_Shader;

		std::vector<TexturePropertyValue> m_Textures;

		size_t m_BufferSize;
		uint8_t* m_Buffer;
	};

	template<>
	Grapple_API TexturePropertyValue& Material::GetPropertyValue(uint32_t index);

	template<>
	Grapple_API TexturePropertyValue Material::ReadPropertyValue(uint32_t index);

	template<>
	Grapple_API void Material::WritePropertyValue(uint32_t index, const TexturePropertyValue& value);
}