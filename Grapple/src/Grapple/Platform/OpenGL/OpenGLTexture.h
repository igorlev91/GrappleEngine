#pragma once

#include "Grapple/Renderer/Texture.h"

#include <glad/glad.h>

namespace Grapple
{
	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const std::filesystem::path& path);
		OpenGLTexture(uint32_t width, uint32_t height, const void* data, TextureFormat format);
		~OpenGLTexture();
	public:
		virtual void Bind(uint32_t slot = 0) override;
		virtual void SetData(const void* data, size_t size) override;

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual TextureFormat GetFormat() const override { return m_Format; }
	private:
		TextureFormat m_Format;
		uint32_t m_Width;
		uint32_t m_Height;

		uint32_t m_Id;
		GLenum m_InternalTextureFormat;
		GLenum m_TextureDataType;
	};
}