#pragma once

#include "GrappleCore/Serialization/TypeInitializer.h"

#include "Grapple/AssetManager/Asset.h"
#include "Grapple/Renderer/Shader.h"
#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Renderer/ShaderMetadata.h"
#include "Grapple/Renderer/ShaderConstantBuffer.h"

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

		template<typename T>
		T& GetPropertyValue(uint32_t index)
		{
			return m_ConstantBuffer.GetProperty<T>(index);
		}

		template<typename T>
		T ReadPropertyValue(uint32_t index) const
		{
			return m_ConstantBuffer.GetProperty<T>(index);
		}

		template<typename T>
		void WritePropertyValue(uint32_t index, const T& value)
		{
			m_ConstantBuffer.SetProperty<T>(index, value);
		}

		const Ref<Texture>& GetTextureProperty(uint32_t propertyIndex) const;
		void SetTextureProperty(uint32_t propertyIndex, Ref<Texture> texture);

		inline uint8_t* GetPropertiesBuffer() { return m_ConstantBuffer.GetBuffer(); }
		inline const uint8_t* GetPropertiesBuffer() const { return m_ConstantBuffer.GetBuffer(); }

		inline ShaderConstantBuffer& GetConstantBuffer() { return m_ConstantBuffer; }
		inline const ShaderConstantBuffer& GetConstantBuffer() const { return m_ConstantBuffer; }
	public:
		static Ref<Material> Create();
		static Ref<Material> Create(Ref<Shader> shader);
		static Ref<Material> Create(AssetHandle shaderHandle);
	private:
		void Initialize();
	protected:
		Ref<Shader> m_Shader;

		ShaderConstantBuffer m_ConstantBuffer;
		std::vector<Ref<Texture>> m_Textures;

		bool m_IsDirty = false;
	};
}