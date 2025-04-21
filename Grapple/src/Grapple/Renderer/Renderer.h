#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/Renderer/RenderData.h"
#include "Grapple/Renderer/Viewport.h"

#include "Grapple/Renderer/VertexArray.h"
#include "Grapple/Renderer/Material.h"

#include <glm/glm.hpp>

namespace Grapple
{
	class Grapple_API Renderer
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void SetMainViewport(Viewport& viewport);

		static void BeginScene(Viewport& viewport);
		static void EndScene();

		static void DrawMesh(const Ref<VertexArray>& mesh, const Ref<Material>& material);

		static Viewport& GetMainViewport();
		static Viewport& GetCurrentViewport();
	};
}