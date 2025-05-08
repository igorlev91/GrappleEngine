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
	enum class MeshRenderFlags : uint8_t
	{
		None = 0,
		DontCastShadows = 1,
	};

	Grapple_IMPL_ENUM_BITFIELD(MeshRenderFlags);

	struct RendererStatistics
	{
		uint32_t DrawCallsCount;
		uint32_t DrawCallsSavedByInstances;
	};

	struct ShadowSettings
	{
		ShadowSettings()
			: Resolution(0),
			LightSize(1.0f),
			Bias(0.0f) {}

		const uint32_t MaxCascades = 4;

		float LightSize;
		float Bias;
		uint32_t Resolution;
		int32_t Cascades;

		float CascadeSplits[4] = { 0.0f };
	};

	class Grapple_API Renderer
	{
	public:
		static void Initialize();
		static void Shutdown();

		static const RendererStatistics& GetStatistics();
		static void ClearStatistics();

		static void SetMainViewport(Viewport& viewport);

		static void BeginScene(Viewport& viewport);
		static void Flush();
		static void EndScene();

		static void DrawFullscreenQuad(const Ref<Material>& material);
		static void DrawMesh(const Ref<VertexArray>& mesh, const Ref<Material>& material, size_t indicesCount = SIZE_MAX);
		static void DrawMesh(const Ref<Mesh>& mesh,
			const Ref<Material>& material,
			const glm::mat4& transform,
			MeshRenderFlags flags = MeshRenderFlags::None,
			int32_t entityIndex = INT32_MAX);

		static void AddRenderPass(Ref<RenderPass> pass);
		static void ExecuteRenderPasses();

		static Viewport& GetMainViewport();
		static Viewport& GetCurrentViewport();

		static Ref<const VertexArray> GetFullscreenQuad();
		static Ref<Texture> GetWhiteTexture();

		static Ref<FrameBuffer> GetShadowsRenderTarget(size_t index);
		static ShadowSettings& GetShadowSettings();
	private:
		static void DrawQueued(bool shadowPass);
		static void FlushInstances();
	};
}