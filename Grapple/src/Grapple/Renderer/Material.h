#pragma once

#include "Grapple/AssetManager/Asset.h"
#include "Grapple/Renderer/Shader.h"
#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Renderer/ShaderMetadata.h"

namespace Grapple
{
	class Grapple_API Material : public Asset
	{
	public:
		Material(Ref<Shader> shader);
		Material(AssetHandle shaderHandle);
		virtual ~Material();

		inline Ref<Shader> GetShader() const { return m_Shader; }
		void SetShader(const Ref<Shader>& shader);

		void SetIntArray(uint32_t index, const int32_t* values, uint32_t count);

		template<typename T>
		T ReadPropertyValue(uint32_t index)
		{
			const ShaderProperties& properties = m_Shader->GetProperties();
			Grapple_CORE_ASSERT((size_t)index < properties.size());
			Grapple_CORE_ASSERT(sizeof(T) == properties[index].Size);

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

			memcpy_s(m_Buffer + properties[index].Offset, sizeof(value), &value, properties[index].Size);
		}

		void SetShaderProperties();
	private:
		void Initialize();
	private:
		Ref<Shader> m_Shader;

		size_t m_BufferSize;
		uint8_t* m_Buffer;
	};
}