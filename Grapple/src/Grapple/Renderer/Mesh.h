#pragma once

#include "Grapple/AssetManager/Asset.h"
#include "Grapple/Renderer/VertexArray.h"
#include "Grapple/Renderer/Buffer.h"
#include "Grapple//Math/Math.h"

namespace Grapple
{
	struct SubMesh
	{
		Ref<VertexArray> MeshVertexArray = nullptr;
		Ref<IndexBuffer> Indicies = nullptr;
		Ref<VertexBuffer> Vertices = nullptr;
		Ref<VertexBuffer> Normals = nullptr;
		Ref<VertexBuffer> UVs = nullptr;

		Ref<VertexBuffer> InstanceBuffer = nullptr;

		Math::AABB Bounds;
	};

	class Grapple_API Mesh : public Asset
	{
	public:
		Mesh();
		Mesh(size_t verticesCount, size_t indicesCount);
		Mesh(const glm::vec3* vertices, size_t verticesCount,
			const uint32_t* indices, size_t indicesCount,
			const glm::vec3* normals,
			const glm::vec2* uvs);

		void SetIndices(const uint32_t* data, size_t count);
		void SetVertices(const glm::vec3* data, size_t count);
		void SetNormals(const glm::vec3* normals, size_t count);
		void SetUVs(const glm::vec2* uvs, size_t count);

		void SetInstanceBuffer(const Ref<VertexBuffer>& instanceBuffer);

		const SubMesh& GetSubMesh() { return m_SubMesh; }
		inline const SubMesh& GetSubMesh() const { return m_SubMesh; }
	private:
		void InitializeBuffers();
	private:
		SubMesh m_SubMesh;
	};
}