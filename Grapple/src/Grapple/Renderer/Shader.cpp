#include "Shader.h"

#include "Grapple/Platform/OpenGL/OpenGLShader.h"
#include "Grapple/Renderer/RendererAPI.h"

namespace Grapple
{
	Ref<Shader> Shader::Create(const std::filesystem::path& path)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLShader>(path);
		}

		return false;
	}
}
