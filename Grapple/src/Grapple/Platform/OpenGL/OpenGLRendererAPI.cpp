#include "OpenGLRendererAPI.h"

#include "Grapple/Core/Log.h"

#include <glad/glad.h>

namespace Grapple
{
	static void OpenGLDebugMessageCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_LOW:
			Grapple_CORE_WARN(message);
			return;
		case GL_DEBUG_SEVERITY_MEDIUM:
			Grapple_CORE_ERROR(message);
			return;
		case GL_DEBUG_SEVERITY_HIGH:
			Grapple_CORE_CRITICAL(message);
			return;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			Grapple_CORE_TRACE(message);
			return;
		}
	}

	void OpenGLRendererAPI::Initialize()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDebugMessageCallback(OpenGLDebugMessageCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		//glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, (int32_t)vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, (const void*)0);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, size_t indicesCount)
	{
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, (int32_t)indicesCount, GL_UNSIGNED_INT, (const void*)0);
	}
}
