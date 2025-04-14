#pragma once

#include "Grapple/Core/Core.h"
#include "Grapple/Renderer/RendererAPI.h"

namespace Grapple
{
	class Grapple_API RenderCommand
	{
	public:
		static void Initialize();
		static void Clear();
		static void SetClearColor(float r, float g, float b, float a);
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void DrawIndexed(const Ref<VertexArray>& mesh);
		static void DrawIndexed(const Ref<VertexArray>& mesh, size_t indicesCount);
	};
}