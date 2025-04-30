#pragma once

#include "Grapple/AssetManager/Asset.h"
#include "Grapple/Renderer/Shader.h"
#include "Grapple/Renderer/RendererAPI.h"

namespace Grapple
{
	struct MaterialFeatures
	{
		MaterialFeatures()
			: Culling(CullingMode::Back), DepthTesting(true) {}

		CullingMode Culling;
		bool DepthTesting;
	};

	class Grapple_API Material : public Asset
	{
	public:
		Material(Ref<Shader> shader);
		Material(AssetHandle shaderHandle);
		virtual ~Material();

		inline Ref<Shader> GetShader() const { return m_Shader; }

		void SetIntArray(uint32_t index, const int32_t* values, uint32_t count);

		template<typename T>
		T ReadParameterValue(uint32_t index)
		{
			const ShaderParameters& parameters = m_Shader->GetParameters();
			Grapple_CORE_ASSERT((size_t)index < parameters.size());
			Grapple_CORE_ASSERT(sizeof(T) == parameters[index].Size);

			T value;

			memcpy_s(&value, sizeof(value), m_Buffer + parameters[index].Offset, parameters[index].Size);
			return value;
		}

		template<typename T>
		void WriteParameterValue(uint32_t index, const T& value)
		{
			const ShaderParameters& parameters = m_Shader->GetParameters();
			Grapple_CORE_ASSERT((size_t)index < parameters.size());
			Grapple_CORE_ASSERT(sizeof(T) == parameters[index].Size);

			memcpy_s(m_Buffer + parameters[index].Offset, sizeof(value), &value, parameters[index].Size);
		}

		void SetShaderParameters();
	private:
		void Initialize();
	public:
		MaterialFeatures Features;
	private:
		Ref<Shader> m_Shader;

		size_t m_BufferSize;
		uint8_t* m_Buffer;
	};
}