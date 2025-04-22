#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/Renderer/RendererAPI.h"

namespace Grapple
{
	class Grapple_API RenderCommand
	{
	public:
		static void Initialize();
		static void Clear();
		static void SetLineWidth(float width);
		static void SetClearColor(float r, float g, float b, float a);
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void DrawIndexed(const Ref<const VertexArray>& mesh);
		static void DrawIndexed(const Ref<const VertexArray>& mesh, size_t indicesCount);
		static void DrawLines(const Ref<const VertexArray>& lines, size_t verticesCount);
		static void SetDepthTestEnabled(bool enabled);
	};
}