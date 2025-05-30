#pragma once

#include "Grapple/Renderer/GraphicsContext.h"

#include "Grapple/Platform/OpenGL/OpenGLCommandBuffer.h"

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
		virtual void Release() override;
		virtual void BeginFrame() override;
		virtual void Present() override;
		virtual void WaitForDevice() override;

		virtual Ref<CommandBuffer> GetCommandBuffer() const override;
	private:
		GLFWwindow* m_Window = nullptr;
		Ref<OpenGLCommandBuffer> m_PrimaryCommandBuffer = nullptr;
	};
}