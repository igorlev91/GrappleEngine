#pragma once

#include "GrappleCore/Serialization/TypeInitializer.h"

#include "Grapple/AssetManager/Asset.h"
#include "Grapple/Renderer/Shader.h"
#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Renderer/ShaderMetadata.h"

namespace Grapple
{
	class Texture;
	class FrameBuffer;
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
		virtual void SetShader(const Ref<Shader>& shader);

		void SetIntArray(uint32_t index, const int32_t* values, uint32_t count);

		template<typename T>
		T& GetPropertyValue(uint32_t index)
		{
			const ShaderProperties& properties = m_Shader->GetProperties();
			Grapple_CORE_ASSERT((size_t)index < properties.size());
			Grapple_CORE_ASSERT(sizeof(T) == properties[index].Size);
			Grapple_CORE_ASSERT(properties[index].Type != ShaderDataType::Sampler);
			Grapple_CORE_ASSERT(properties[index].Offset + properties[index].Size <= m_BufferSize);

			m_IsDirty = true;
			return *(T*)(m_Buffer + properties[index].Offset);
		}

		template<typename T>
		T ReadPropertyValue(uint32_t index) const
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

			m_IsDirty = true;
		}

		const Ref<Texture>& GetTextureProperty(uint32_t propertyIndex) const;
		void SetTextureProperty(uint32_t propertyIndex, Ref<Texture> texture);

		inline uint8_t* GetPropertiesBuffer() { return m_Buffer; }
		inline const uint8_t* GetPropertiesBuffer() const { return m_Buffer; }
	public:
		static Ref<Material> Create();
		static Ref<Material> Create(Ref<Shader> shader);
		static Ref<Material> Create(AssetHandle shaderHandle);
	private:
		void Initialize();
	protected:
		Ref<Shader> m_Shader;

		std::vector<Ref<Texture>> m_Textures;

		size_t m_BufferSize;
		uint8_t* m_Buffer;

		bool m_IsDirty = false;
	};
}