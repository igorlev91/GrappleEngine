#pragma once

#include "Grapple/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Grapple
{
	class OpenGLGraphicsContext : public GraphicsContext
	{
	public:
		OpenGLGraphicsContext(GLFWwindow* windowHandle);
	public:
		virtual void Initialize() override;
		virtual void Present() override;
	private:
		GLFWwindow* m_Window;
	};
}