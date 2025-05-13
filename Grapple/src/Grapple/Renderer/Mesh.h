#pragma once

#include "Grapple/AssetManager/Asset.h"
#include "Grapple/Renderer/VertexArray.h"
#include "Grapple/Renderer/Buffer.h"
#include "Grapple/Math/Math.h"

#include "GrappleCore/Collections/Span.h"

namespace Grapple
{
	struct SubMesh
	{
		Math::AABB Bounds;

		Ref<VertexArray> VertexArray = nullptr;
		Ref<IndexBuffer> Indicies = nullptr;
		Ref<VertexBuffer> Vertices = nullptr;
		Ref<VertexBuffer> Normals = nullptr;
		Ref<VertexBuffer> UVs = nullptr;
	};

	class Grapple_API Mesh : public Asset
	{
	public:
		Mesh();

		void AddSubMesh(const Span<glm::vec3>& vertices,
			IndexBuffer::IndexFormat indexFormat,
			const MemorySpan& indices,
			const Span<glm::vec3>* normals = nullptr,
			const Span<glm::vec2>* uvs = nullptr);

		inline Ref<VertexBuffer> GetInstanceBuffer() const { return m_InstanceBuffer; }
		void SetInstanceBuffer(const Ref<VertexBuffer>& instanceBuffer);

		inline const std::vector<SubMesh>& GetSubMeshes() const { return m_SubMeshes; }
	private:
		void InitializeLastSubMeshBuffers();
	private:
		Ref<VertexBuffer> m_InstanceBuffer = nullptr;
		std::vector<SubMesh> m_SubMeshes;
	};
}