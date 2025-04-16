#include "RenderCommand.h"

namespace Grapple
{
	Scope<RendererAPI> s_API = RendererAPI::Create();

	void RenderCommand::Initialize()
	{
		s_API->Initialize();
	}

	void RenderCommand::Clear()
	{
		s_API->Clear();
	}

	void RenderCommand::SetLineWidth(float width)
	{
		s_API->SetLineWidth(width);
	}

	void RenderCommand::SetClearColor(float r, float g, float b, float a)
	{
		s_API->SetClearColor(r, g, b, a);
	}

	void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		s_API->SetViewport(x, y, width, height);
	}

	void RenderCommand::DrawIndexed(const Ref<VertexArray>& mesh)
	{
		s_API->DrawIndexed(mesh);
	}

	void RenderCommand::DrawIndexed(const Ref<VertexArray>& mesh, size_t indicesCount)
	{
		s_API->DrawIndexed(mesh, indicesCount);
	}

	void RenderCommand::DrawLines(const Ref<VertexArray>& lines, size_t verticesCount)
	{
		s_API->DrawLines(lines, verticesCount);
	}

	void RenderCommand::SetDepthTestEnabled(bool enabled)
	{
		s_API->SetDepthTestEnabled(enabled);
	}
}