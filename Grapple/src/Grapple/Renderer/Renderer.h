#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/Renderer/RenderData.h"
#include "Grapple/Renderer/Viewport.h"
#include "Grapple/Renderer/RenderPass.h"

#include "Grapple/Renderer/VertexArray.h"
#include "Grapple/Renderer/Material.h"
#include "Grapple/Renderer/Mesh.h"

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

		static Ref<const VertexArray> GetFullscreenQuad();

		static void DrawMesh(const Ref<VertexArray>& mesh, const Ref<Material>& material, size_t indicesCount = SIZE_MAX);
		static void DrawMesh(const Ref<Mesh>& mesh, const Ref<Material>& material, const glm::mat4& transform);

		static void AddRenderPass(Scope<RenderPass> pass);
		static void ExecuteRenderPasses();

		static Viewport& GetMainViewport();
		static Viewport& GetCurrentViewport();
	};
}