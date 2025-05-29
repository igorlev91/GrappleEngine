#pragma once

#include "Grapple/Renderer/Mesh.h"
#include "Grapple/Platform/OpenGL/OpenGLVertexArray.h"

namespace Grapple
{
	class OpenGLMesh : public Mesh
	{
	public:
		OpenGLMesh(MeshTopology topologyType,
			size_t vertexBufferSize,
			IndexBuffer::IndexFormat indexFormat,
			size_t indexBufferSize);

		void AddSubMesh(const Span<glm::vec3>& vertices,
			const MemorySpan& indices,
			const Span<glm::vec3>& normals,
			const Span<glm::vec3>& tangents,
			const Span<glm::vec2>& uvs) override;

		inline const OpenGLVertexArray& GetVertexArray() const { return m_VertexArray; }
	private:
		OpenGLVertexArray m_VertexArray;
	};
}
