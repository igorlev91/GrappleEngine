#include "OpenGLRendererAPI.h"

#include "GrappleCore/Log.h"

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

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);

		glFrontFace(GL_CW);
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

	void OpenGLRendererAPI::SetDepthTestEnabled(bool enabled)
	{
		if (enabled)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetCullingMode(CullingMode mode)
	{
		switch (mode)
		{
		case CullingMode::None:
			glDisable(GL_CULL_FACE);
			break;
		case CullingMode::Front:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			break;
		case CullingMode::Back:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;
		}
	}

	void OpenGLRendererAPI::SetDepthComparisonFunction(DepthComparisonFunction function)
	{
		switch (function)
		{
		case DepthComparisonFunction::Less:
			glDepthFunc(GL_LESS);
			break;
		case DepthComparisonFunction::Greater:
			glDepthFunc(GL_GREATER);
			break;
		case DepthComparisonFunction::LessOrEqual:
			glDepthFunc(GL_LEQUAL);
			break;
		case DepthComparisonFunction::GreaterOrEqual:
			glDepthFunc(GL_GEQUAL);
			break;
		case DepthComparisonFunction::Equal:
			glDepthFunc(GL_EQUAL);
			break;
		case DepthComparisonFunction::NotEqual:
			glDepthFunc(GL_NOTEQUAL);
			break;
		case DepthComparisonFunction::Always:
			glDepthFunc(GL_ALWAYS);
			break;
		case DepthComparisonFunction::Never:
			glDepthFunc(GL_LESS);
			break;
		}
	}

	void OpenGLRendererAPI::SetDepthWriteEnabled(bool enabled)
	{
		if (enabled)
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);
	}

	void OpenGLRendererAPI::SetLineWidth(float width)
	{
		glLineWidth(width);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<const VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, (int32_t)vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, (const void*)0);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<const VertexArray>& vertexArray, size_t indicesCount)
	{
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, (int32_t)indicesCount, GL_UNSIGNED_INT, (const void*)0);
	}

	void OpenGLRendererAPI::DrawInstanced(const Ref<const VertexArray>& mesh, size_t instancesCount)
	{
		mesh->Bind();
		glDrawElementsInstanced(GL_TRIANGLES, (int32_t)mesh->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, (const void*)0, (int32_t)instancesCount);
		mesh->Unbind();
	}

	void OpenGLRendererAPI::DrawInstanced(const Ref<const VertexArray>& mesh, size_t instancesCount, size_t baseVertexIndex, size_t startIndex, size_t indicesCount)
	{
		mesh->Bind();
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, (int32_t)indicesCount, GL_UNSIGNED_INT, (const void*)0, (int32_t)instancesCount, (int32_t)baseVertexIndex);
		mesh->Unbind();
	}

	void OpenGLRendererAPI::DrawLines(const Ref<const VertexArray>& vertexArray, size_t verticesCount)
	{
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, (int32_t)verticesCount);
	}
}
