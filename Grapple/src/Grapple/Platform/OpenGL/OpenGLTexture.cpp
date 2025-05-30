#include "OpenGLTexture.h"

#include "Grapple.h"

#include <glad/glad.h>
#include <stb_image/stb_image.h>

namespace Grapple
{
	OpenGLTexture::OpenGLTexture(const std::filesystem::path& path, const TextureSpecifications& specifications)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

		if (data)
		{
			m_Specifications = specifications;
			m_Specifications.Width = width;
			m_Specifications.Height = height;

			// Don't generate mip maps for small textures
			if (m_Specifications.Width < 512 || m_Specifications.Height < 512)
				m_Specifications.GenerateMipMaps = false;

			m_InternalTextureFormat = 0;
			m_TextureDataType = 0;

			if (channels == 3)
			{
				m_InternalTextureFormat = GL_RGB8;
				m_TextureDataType = GL_RGB;
				m_Specifications.Format = TextureFormat::RGB8;
			}
			else if (channels == 4)
			{
				m_InternalTextureFormat = GL_RGBA8;
				m_TextureDataType = GL_RGBA;
				m_Specifications.Format = TextureFormat::RGBA8;
			}

			glCreateTextures(GL_TEXTURE_2D, 1, &m_Id);
			glTextureStorage2D(m_Id,
				m_Specifications.GenerateMipMaps
					? TextureSpecifications::DefaultMipLevelsCount
					: 1,
				m_InternalTextureFormat,
				m_Specifications.Width,
				m_Specifications.Height);

			if (m_Specifications.GenerateMipMaps)
			{
				switch (m_Specifications.Filtering)
				{
				case TextureFiltering::Closest:
					glTextureParameteri(m_Id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
					glTextureParameteri(m_Id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					break;
				case TextureFiltering::Linear:
					glTextureParameteri(m_Id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTextureParameteri(m_Id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					break;
				}
			}
			else
			{
				switch (m_Specifications.Filtering)
				{
				case TextureFiltering::Closest:
					glTextureParameteri(m_Id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTextureParameteri(m_Id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					break;
				case TextureFiltering::Linear:
					glTextureParameteri(m_Id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTextureParameteri(m_Id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					break;
				}
			}

			switch (m_Specifications.Wrap)
			{
			case TextureWrap::Clamp:
				glTextureParameteri(m_Id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTextureParameteri(m_Id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(m_Id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
			case TextureWrap::Repeat:
				glTextureParameteri(m_Id, GL_TEXTURE_WRAP_R, GL_REPEAT);
				glTextureParameteri(m_Id, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTextureParameteri(m_Id, GL_TEXTURE_WRAP_T, GL_REPEAT);
				break;
			}

			glTextureSubImage2D(m_Id, 0, 0, 0, m_Specifications.Width, m_Specifications.Height, m_TextureDataType, GL_UNSIGNED_BYTE, data);

			if (m_Specifications.GenerateMipMaps)
				glGenerateTextureMipmap(m_Id);

			stbi_image_free(data);
		}
		else
			Grapple_CORE_ERROR("Could not load an image {0}", path.string());
	}

	OpenGLTexture::OpenGLTexture(uint32_t width, uint32_t height, const void* data, TextureFormat format, TextureFiltering filtering)
	{
		GLenum dataType = GL_UNSIGNED_BYTE;
		switch (m_Specifications.Format)
		{
		case TextureFormat::RGB8:
		case TextureFormat::RGBA8:
		case TextureFormat::RG8:
		case TextureFormat::R8:
			dataType = GL_UNSIGNED_BYTE;
			break;
		case TextureFormat::RF32:
			dataType = GL_FLOAT;
			break;
		case TextureFormat::RG16:
			dataType = GL_UNSIGNED_SHORT;
			break;
		}

		switch (format)
		{
		case TextureFormat::RGB8:
			m_InternalTextureFormat = GL_RGB8;
			m_TextureDataType = GL_RGB;
			break;
		case TextureFormat::RGBA8:
			m_InternalTextureFormat = GL_RGBA8;
			m_TextureDataType = GL_RGBA;
			break;
		case TextureFormat::RF32:
			m_InternalTextureFormat = GL_R32F;
			m_TextureDataType = GL_RED;
			break;
		case TextureFormat::RG8:
			m_InternalTextureFormat = GL_RG8;
			m_TextureDataType = GL_RG;
			break;
		case TextureFormat::R8:
			m_InternalTextureFormat = GL_R8;
			m_TextureDataType = GL_RED;
			break;
		case TextureFormat::RG16:
			m_InternalTextureFormat = GL_RG16;
			m_TextureDataType = GL_RG;
			break;
		}

		m_Specifications.Width = width;
		m_Specifications.Height = height;
		m_Specifications.Filtering = filtering;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_Id);
		glTextureStorage2D(m_Id, 1, m_InternalTextureFormat, m_Specifications.Width, m_Specifications.Height);

		SetFiltering(m_Specifications.Filtering);

		glTextureSubImage2D(m_Id, 0, 0, 0, m_Specifications.Width, m_Specifications.Height, m_TextureDataType, dataType, data);
	}
	
	OpenGLTexture::~OpenGLTexture()
	{
		glDeleteTextures(1, &m_Id);
	}

	void OpenGLTexture::Bind(uint32_t slot)
	{
		glBindTextureUnit(slot, m_Id);
	}

	void OpenGLTexture::SetData(const void* data, size_t size)
	{
		GLenum dataType = GL_UNSIGNED_BYTE;
		switch (m_Specifications.Format)
		{
		case TextureFormat::RGB8:
		case TextureFormat::RGBA8:
			dataType = GL_UNSIGNED_BYTE;
			break;
		case TextureFormat::RF32:
			dataType = GL_FLOAT;
			break;
		case TextureFormat::RG8:
			dataType = GL_UNSIGNED_BYTE;
			break;
		case TextureFormat::RG16:
			dataType = GL_UNSIGNED_SHORT;
			break;
		}

		glTextureSubImage2D(m_Id, 0, 0, 0, m_Specifications.Width, m_Specifications.Height, m_TextureDataType, dataType, data);
	}

	void OpenGLTexture::SetFiltering(TextureFiltering filtering)
	{
		switch (m_Specifications.Filtering)
		{
		case TextureFiltering::Closest:
			glTextureParameteri(m_Id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(m_Id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;
		case TextureFiltering::Linear:
			glTextureParameteri(m_Id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_Id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		}
	}



	OpenGLTexture3D::OpenGLTexture3D(const Texture3DSpecifications& specifications)
		: m_Id(0), m_InternalTextureFormat(0), m_TextureDataType(0), m_Specifications(specifications)
	{
		CreateTexture();
	}

	OpenGLTexture3D::OpenGLTexture3D(const Texture3DSpecifications& specifications, const void* data, glm::uvec3 dataSize)
		: m_Id(0), m_InternalTextureFormat(0), m_TextureDataType(0), m_Specifications(specifications)
	{
		CreateTexture();
		SetData(data, dataSize);
	}

	void OpenGLTexture3D::Bind(uint32_t slot)
	{
		glBindTextureUnit(slot, m_Id);
	}

	void* OpenGLTexture3D::GetRendererId() const
	{
		return (void*)(size_t)m_Id;
	}

	void OpenGLTexture3D::SetData(const void* data, glm::uvec3 dataSize)
	{
		GLenum dataType = GL_UNSIGNED_BYTE;

		switch (m_Specifications.Format)
		{
		case TextureFormat::RGB8:
		case TextureFormat::RGBA8:
			dataType = GL_UNSIGNED_BYTE;
			break;
		case TextureFormat::RF32:
			dataType = GL_FLOAT;
			break;
		case TextureFormat::RG8:
			dataType = GL_UNSIGNED_BYTE;
			break;
		case TextureFormat::RG16:
			dataType = GL_UNSIGNED_SHORT;
			break;
		}

		glTextureSubImage3D(m_Id, 0, 0, 0, 0,
			dataSize.x, dataSize.y, dataSize.z,
			m_TextureDataType, dataType, data);
	}

	const Texture3DSpecifications& OpenGLTexture3D::GetSpecifications() const
	{
		return m_Specifications;
	}

	void OpenGLTexture3D::CreateTexture()
	{
		switch (m_Specifications.Format)
		{
		case TextureFormat::RGB8:
			m_InternalTextureFormat = GL_RGB8;
			m_TextureDataType = GL_RGB;
			break;
		case TextureFormat::RGBA8:
			m_InternalTextureFormat = GL_RGBA8;
			m_TextureDataType = GL_RGBA;
			break;
		case TextureFormat::RF32:
			m_InternalTextureFormat = GL_R32F;
			m_TextureDataType = GL_RED;
			break;
		}

		glCreateTextures(GL_TEXTURE_3D, 1, &m_Id);
		glTextureStorage3D(m_Id, 1, m_InternalTextureFormat, m_Specifications.Size.x, m_Specifications.Size.y, m_Specifications.Size.z);

		switch (m_Specifications.Filtering)
		{
		case TextureFiltering::Closest:
			glTextureParameteri(m_Id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(m_Id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;
		case TextureFiltering::Linear:
			glTextureParameteri(m_Id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_Id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		}

		switch (m_Specifications.Wrap)
		{
		case TextureWrap::Clamp:
			glTextureParameteri(m_Id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTextureParameteri(m_Id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(m_Id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			break;
		case TextureWrap::Repeat:
			glTextureParameteri(m_Id, GL_TEXTURE_WRAP_R, GL_REPEAT);
			glTextureParameteri(m_Id, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_Id, GL_TEXTURE_WRAP_T, GL_REPEAT);
			break;
		}
	}
}