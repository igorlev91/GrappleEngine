#pragma once

#include "Grapple/Renderer/Texture.h"

namespace Grapple
{
	class OpenGLTexture : public Texture
	{
	public:
		// Texture size and format are ignored, because they are retrieved from the file
		OpenGLTexture(const std::filesystem::path& path, const TextureSpecifications& specifications);
		OpenGLTexture(uint32_t width, uint32_t height, const void* data, TextureFormat format, TextureFiltering filtering);
		OpenGLTexture(const TextureSpecifications& specifications, const void* data);
		~OpenGLTexture();
	public:
		uint32_t GetId() const { return m_Id; }

		virtual void Bind(uint32_t slot = 0) override;
		virtual void SetData(const void* data, size_t size) override;

		virtual const TextureSpecifications& GetSpecifications() const override { return m_Specifications; }

		virtual uint32_t GetWidth() const override { return m_Specifications.Width; }
		virtual uint32_t GetHeight() const override { return m_Specifications.Height; }
		virtual TextureFormat GetFormat() const override { return m_Specifications.Format; }
		virtual TextureFiltering GetFiltering() const override { return m_Specifications.Filtering; }
	private:
		void SetFiltering(TextureFiltering filtering);
	private:
		uint32_t m_Id;
		TextureSpecifications m_Specifications;

		uint32_t m_InternalTextureFormat;
		uint32_t m_TextureDataType;
	};



	class OpenGLTexture3D : public Texture3D
	{
	public:
		OpenGLTexture3D(const Texture3DSpecifications& specifications);
		OpenGLTexture3D(const Texture3DSpecifications& specifications, const void* data, glm::uvec3 dataSize);
	public:
		virtual void Bind(uint32_t slot) override;
		virtual void* GetRendererId() const override;
		virtual void SetData(const void* data, glm::uvec3 dataSize);
		virtual const Grapple::Texture3DSpecifications& GetSpecifications() const;
	private:
		void CreateTexture();
	private:
		uint32_t m_Id;

		Texture3DSpecifications m_Specifications;

		uint32_t m_InternalTextureFormat;
		uint32_t m_TextureDataType;
	};
}