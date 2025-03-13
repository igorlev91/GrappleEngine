#include "Texture.h"

#include <Grapple/Renderer/RendererAPI.h>

#include <Grapple/Platform/OpenGL/OpenGLTexture.h>

namespace Grapple
{
	Ref<Texture> Texture::Create(const std::filesystem::path& path)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture>(path);
		}
	}

	Ref<Texture> Texture::Create(uint32_t width, uint32_t height, const void* data, TextureFormat format)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture>(width, height, data, format);
		}
	}
}