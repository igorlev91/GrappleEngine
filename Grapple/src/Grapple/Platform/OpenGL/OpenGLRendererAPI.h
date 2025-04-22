#pragma once

#include "Grapple/Renderer/RendererAPI.h"

namespace Grapple
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Initialize() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void SetClearColor(float r, float g, float b, float a) override;
		virtual void Clear() override;

		virtual void SetDepthTestEnabled(bool enabled) override;

		virtual void SetLineWidth(float width) override;

		virtual void DrawIndexed(const Ref<const VertexArray>& vertexArray) override;
		virtual void DrawIndexed(const Ref<const VertexArray>& vertexArray, size_t indicesCount) override;
		virtual void DrawLines(const Ref<const VertexArray>& vertexArray, size_t verticesCount) override;
	};
}