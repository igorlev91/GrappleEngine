#include "OpenGLGraphicsContext.h"

#include "Grapple.h"

namespace Grapple
{
	OpenGLGraphicsContext::OpenGLGraphicsContext(GLFWwindow* windowHandle)
		: m_Window(windowHandle)
	{
		m_PrimaryCommandBuffer = CreateRef<OpenGLCommandBuffer>();
	}

	void OpenGLGraphicsContext::Initialize()
	{
		glfwMakeContextCurrent(m_Window);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (!status)
			Grapple_CORE_CRITICAL("Failed to initialize GLAD");
	}

	void OpenGLGraphicsContext::Release() {}

	void OpenGLGraphicsContext::BeginFrame() {}

	void OpenGLGraphicsContext::Present()
	{
		glfwSwapBuffers(m_Window);
	}

	void OpenGLGraphicsContext::WaitForDevice() {}

	Ref<CommandBuffer> OpenGLGraphicsContext::GetCommandBuffer() const
	{
		return m_PrimaryCommandBuffer;
	}
}
