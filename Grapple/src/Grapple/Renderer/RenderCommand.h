#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Renderer/Mesh.h"

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
		static void DrawIndexed(const Ref<const VertexArray>& mesh, size_t firstIndex, size_t indicesCount);
		static void DrawInstanced(const Ref<const VertexArray>& mesh, size_t instancesCount);

		static void DrawInstancesIndexed(const Ref<const Mesh>& mesh,
			uint32_t subMeshIndex,
			uint32_t instancesCount,
			uint32_t baseInstance);

		static void DrawInstancesIndexedIndirect(
			const Ref<const Mesh>& mesh,
			const Span<DrawIndirectCommandSubMeshData>& subMeshesData,
			uint32_t baseInstance);

		static void DrawInstanced(const Ref<const VertexArray>& mesh,
			size_t instancesCount,
			size_t baseVertexIndex,
			size_t startIndex,
			size_t indicesCount);

		static void DrawLines(const Ref<const VertexArray>& lines, size_t verticesCount);
		static void SetDepthTestEnabled(bool enabled);
		static void SetCullingMode(CullingMode mode);
		static void SetDepthComparisonFunction(DepthComparisonFunction function);
		static void SetDepthWriteEnabled(bool enabled);
		static void SetBlendMode(BlendMode mode);
		static void ApplyMaterial(const Ref<const Material>& materail);
	};
}