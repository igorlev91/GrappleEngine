#include "OpenGLGraphicsContext.h"

#include "Grapple.h"

namespace Grapple
{
	OpenGLGraphicsContext::OpenGLGraphicsContext(GLFWwindow* windowHandle)
		: m_Window(windowHandle)
	{

	}

	void OpenGLGraphicsContext::Initialize()
	{
		glfwMakeContextCurrent(m_Window);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (!status)
			Grapple_CORE_CRITICAL("Failed to initialize GLAD");
	}

	void OpenGLGraphicsContext::SwapBuffers()
	{
		glfwSwapBuffers(m_Window);
	}
}
