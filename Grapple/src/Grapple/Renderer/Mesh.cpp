#include "Mesh.h"

#include "GrappleCore/Profiler/Profiler.h"

#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Renderer/GraphicsContext.h"

#include "Grapple/Platform/Vulkan/VulkanContext.h"

namespace Grapple
{
	Grapple_SERIALIZABLE_IMPL(Mesh);
	Grapple_IMPL_ASSET(Mesh);

	Mesh::Mesh(size_t vertexBufferSize, IndexBuffer::IndexFormat indexFormat, size_t indexBufferSize)
		: Asset(AssetType::Mesh),
		m_VertexBufferSize(vertexBufferSize),
		m_IndexFormat(indexFormat),
		m_IndexBufferSize(indexBufferSize)
	{
	}

	Mesh::Mesh(MemorySpan indices,
		IndexBuffer::IndexFormat indexFormat,
		Span<glm::vec3> vertices,
		Span<glm::vec3> normals,
		Span<glm::vec3> tangents,
		Span<glm::vec2> uvs)
		: Asset(AssetType::Mesh),
		m_IndexFormat(indexFormat),
		m_VertexBufferSize(vertices.GetSize()),
		m_VertexBufferOffset(0),
		m_IndexBufferSize(indices.GetSize()),
		m_IndexBufferOffset(0)
	{
		Grapple_PROFILE_FUNCTION();
		Grapple_CORE_ASSERT(vertices.GetSize() == normals.GetSize());
		Grapple_CORE_ASSERT(vertices.GetSize() == tangents.GetSize());
		Grapple_CORE_ASSERT(vertices.GetSize() == uvs.GetSize());

		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{
			Ref<CommandBuffer> commandBuffer = VulkanContext::GetInstance().BeginTemporaryCommandBuffer();

			m_Vertices = VertexBuffer::Create(sizeof(glm::vec3) * vertices.GetSize(), vertices.GetData(), commandBuffer);
			m_Normals = VertexBuffer::Create(sizeof(glm::vec3) * normals.GetSize(), normals.GetData(), commandBuffer);
			m_Tangents = VertexBuffer::Create(sizeof(glm::vec3) * tangents.GetSize(), tangents.GetData(), commandBuffer);
			m_UVs = VertexBuffer::Create(sizeof(glm::vec2) * uvs.GetSize(), uvs.GetData(), commandBuffer);

			m_IndexBuffer = IndexBuffer::Create(m_IndexFormat, indices, commandBuffer);

			VulkanContext::GetInstance().EndTemporaryCommandBuffer(As<VulkanCommandBuffer>(commandBuffer));
		}
		else
		{
			m_Vertices = VertexBuffer::Create(sizeof(glm::vec3) * vertices.GetSize(), vertices.GetData());
			m_Normals = VertexBuffer::Create(sizeof(glm::vec3) * normals.GetSize(), normals.GetData());
			m_Tangents = VertexBuffer::Create(sizeof(glm::vec3) * tangents.GetSize(), tangents.GetData());
			m_UVs = VertexBuffer::Create(sizeof(glm::vec2) * uvs.GetSize(), uvs.GetData());

			m_IndexBuffer = IndexBuffer::Create(m_IndexFormat, indices);
		}
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::AddSubMesh(const Span<glm::vec3>& vertices,
		const MemorySpan& indices,
		const Span<glm::vec3>& normals,
		const Span<glm::vec3>& tangents,
		const Span<glm::vec2>& uvs)
	{
		Grapple_PROFILE_FUNCTION();
		Grapple_CORE_ASSERT(vertices.GetSize() > 0);
		Grapple_CORE_ASSERT(vertices.GetSize() == normals.GetSize());
		Grapple_CORE_ASSERT(vertices.GetSize() == uvs.GetSize());

		size_t indexSize = 0;
		switch (m_IndexFormat)
		{
		case IndexBuffer::IndexFormat::UInt16:
			indexSize = sizeof(uint16_t);
			break;
		case IndexBuffer::IndexFormat::UInt32:
			indexSize = sizeof(uint32_t);
			break;
		}

		SubMesh& subMesh = m_SubMeshes.emplace_back();
		subMesh.Bounds = Math::AABB(vertices[0], vertices[0]);
		for (size_t i = 1; i < vertices.GetSize(); i++)
		{
			subMesh.Bounds.Min = glm::min(subMesh.Bounds.Min, vertices[i]);
			subMesh.Bounds.Max = glm::max(subMesh.Bounds.Max, vertices[i]);
		}

		if (!m_IndexBuffer)
			m_IndexBuffer = IndexBuffer::Create(m_IndexFormat, m_IndexBufferSize, GPUBufferUsage::Static);

		if (!m_Vertices)
			m_Vertices = VertexBuffer::Create(m_VertexBufferSize * sizeof(glm::vec3), GPUBufferUsage::Static);

		if (!m_Normals)
			m_Normals = VertexBuffer::Create(m_VertexBufferSize * sizeof(glm::vec3), GPUBufferUsage::Static);

		if (!m_Tangents)
			m_Tangents = VertexBuffer::Create(m_VertexBufferSize * sizeof(glm::vec3), GPUBufferUsage::Static);

		if (!m_UVs)
			m_UVs = VertexBuffer::Create(m_VertexBufferSize * sizeof(glm::vec2), GPUBufferUsage::Static);

		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{
			Ref<CommandBuffer> commandBuffer = VulkanContext::GetInstance().BeginTemporaryCommandBuffer();

			m_Vertices->SetData(MemorySpan(const_cast<glm::vec3*>(vertices.GetData()), vertices.GetSize()), m_VertexBufferOffset * sizeof(glm::vec3), commandBuffer);
			m_Normals->SetData(MemorySpan(const_cast<glm::vec3*>(normals.GetData()), normals.GetSize()), m_VertexBufferOffset * sizeof(glm::vec3), commandBuffer);
			m_Tangents->SetData(MemorySpan(const_cast<glm::vec3*>(tangents.GetData()), tangents.GetSize()), m_VertexBufferOffset * sizeof(glm::vec3), commandBuffer);
			m_UVs->SetData(MemorySpan(const_cast<glm::vec2*>(uvs.GetData()), uvs.GetSize()), m_VertexBufferOffset * sizeof(glm::vec2), commandBuffer);

			m_IndexBuffer->SetData(indices, m_IndexBufferOffset, commandBuffer);

			VulkanContext::GetInstance().EndTemporaryCommandBuffer(As<VulkanCommandBuffer>(commandBuffer));
		}

		size_t indicesCount = indices.GetSize() / indexSize;

		subMesh.BaseIndex = (uint32_t)(m_IndexBufferOffset / indexSize);
		subMesh.BaseVertex = (uint32_t)m_VertexBufferOffset;
		subMesh.IndicesCount = (uint32_t)indicesCount;

		m_VertexBufferOffset += vertices.GetSize();
		m_IndexBufferOffset += indices.GetSize();
	}

	void Mesh::AddSubMesh(const SubMesh& subMesh)
	{
		if (m_SubMeshes.size() == 0)
		{
			m_Bounds = subMesh.Bounds;
		}
		else
		{
			m_Bounds.Min = glm::min(subMesh.Bounds.Min, m_Bounds.Min);
			m_Bounds.Max = glm::max(subMesh.Bounds.Max, m_Bounds.Max);
		}

		m_SubMeshes.push_back(subMesh);
	}

	Ref<Mesh> Mesh::Create(size_t vertexBufferSize, IndexBuffer::IndexFormat indexFormat, size_t indexBufferSize)
	{
		Grapple_PROFILE_FUNCTION();

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			return CreateRef<Mesh>(vertexBufferSize, indexFormat, indexBufferSize);
		}

		Grapple_CORE_ASSERT(false);
		return nullptr;
	}

	Ref<Mesh> Mesh::Create(MemorySpan indices,
		IndexBuffer::IndexFormat indexFormat,
		Span<glm::vec3> vertices,
		Span<glm::vec3> normals,
		Span<glm::vec3> tangents,
		Span<glm::vec2> uvs)
	{
		Grapple_PROFILE_FUNCTION();

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			return CreateRef<Mesh>(indices, indexFormat, vertices, normals, tangents, uvs);
		}

		Grapple_CORE_ASSERT(false);
		return nullptr;
	}
}
