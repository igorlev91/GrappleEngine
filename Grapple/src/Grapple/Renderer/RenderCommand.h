#pragma once

#include <Grapple/Renderer/RendererAPI.h>

namespace Grapple
{
	class RenderCommand
	{
	public:
		static void Initialize()
		{
			s_API->Initialize();
		}

		static void Clear()
		{
			s_API->Clear();
		}

		static void SetClearColor(float r, float g, float b, float a)
		{
			s_API->SetClearColor(r, g, b, a);
		}

		static void DrawIndex(const Ref<VertexArray> mesh)
		{
			s_API->DrawIndexed(mesh);
		}
	private:
		static Scope<RendererAPI> s_API;
	};
}