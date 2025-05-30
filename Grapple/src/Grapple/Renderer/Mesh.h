#pragma once

#include "Grapple/AssetManager/Asset.h"
#include "Grapple/Renderer/VertexArray.h"
#include "Grapple/Renderer/Buffer.h"
#include "Grapple/Math/Math.h"

#include "GrappleCore/Collections/Span.h"
#include "GrappleCore/Serialization/Metadata.h"

namespace Grapple
{
	struct SubMesh
	{
		Math::AABB Bounds;

		uint32_t BaseIndex = 0;
		uint32_t IndicesCount = 0;
		uint32_t BaseVertex = 0;
	};

	enum class MeshRenderFlags : uint8_t
	{
		None = 0,
		DontCastShadows = 1,
	};

	Grapple_IMPL_ENUM_BITFIELD(MeshRenderFlags);

	class Grapple_API Mesh : public Asset
	{
	public:
		Grapple_SERIALIZABLE;
		Grapple_ASSET;

		Mesh(MeshTopology topologyType,
			size_t vertexBufferSize,
			IndexBuffer::IndexFormat indexFormat,
			size_t indexBufferSize);

		virtual void AddSubMesh(const Span<glm::vec3>& vertices,
			const MemorySpan& indices,
			const Span<glm::vec3>& normals,
			const Span<glm::vec3>& tangents,
			const Span<glm::vec2>& uvs);

		constexpr size_t GetVertexBufferSize() const { return m_VertexBufferSize; }
		constexpr size_t GetIndexBufferSize() const { return m_IndexBufferSize; }

		inline Ref<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }
		inline Ref<VertexBuffer> GetVertices() const { return m_Vertices; }
		inline Ref<VertexBuffer> GetNormals() const { return m_Normals; }
		inline Ref<VertexBuffer> GetTangents() const { return m_Tangents; }
		inline Ref<VertexBuffer> GetUVs() const { return m_UVs; }

		inline const std::vector<SubMesh>& GetSubMeshes() const { return m_SubMeshes; }
		inline MeshTopology GetTopologyType() const { return m_TopologyType; }
		inline IndexBuffer::IndexFormat GetIndexFormat() const { return m_IndexFormat; }
	public:
		static Ref<Mesh> Create(MeshTopology topology, size_t vertexBufferSize, IndexBuffer::IndexFormat indexFormat, size_t indexBufferSize);
	protected:
		IndexBuffer::IndexFormat m_IndexFormat;

		size_t m_VertexBufferSize = 0;
		size_t m_IndexBufferSize = 0;

		size_t m_VertexBufferOffset = 0;
		size_t m_IndexBufferOffset = 0;

		Ref<IndexBuffer> m_IndexBuffer = nullptr;
		Ref<VertexBuffer> m_Vertices = nullptr;
		Ref<VertexBuffer> m_Normals = nullptr;
		Ref<VertexBuffer> m_Tangents = nullptr;
		Ref<VertexBuffer> m_UVs = nullptr;

		MeshTopology m_TopologyType;
		std::vector<SubMesh> m_SubMeshes;
	};
}