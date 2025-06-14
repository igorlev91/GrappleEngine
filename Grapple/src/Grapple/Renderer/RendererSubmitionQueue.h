#pragma once

#include "GrappleCore/Profiler/Profiler.h"

#include "Grapple/Renderer/Mesh.h"
#include "Grapple/Renderer/Material.h"
#include "Grapple/Math/Transform.h"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include <stdint.h>

namespace Grapple
{
	class Renderer;
	class RendererSubmitionQueue
	{
	public:
		struct Item
		{
			Ref<const Mesh> Mesh;
			Ref<const Material> Material;
			uint32_t SubMeshIndex;
			Math::Compact3DTransform Transform;
			MeshRenderFlags Flags;

			float SortKey;
		};

		void Submit(const Ref<const Mesh>& mesh,
			uint32_t subMesh,
			const Ref<const Material>& material,
			const glm::mat4& transform,
			MeshRenderFlags flags,
			int32_t entityIndex)
		{
			Submit(mesh, subMesh, material, Math::Compact3DTransform(transform), flags, entityIndex);
		}

		void Submit(const Ref<const Mesh>& mesh,
			uint32_t subMesh,
			const Ref<const Material>& material,
			const Math::Compact3DTransform& transform,
			MeshRenderFlags flags,
			int32_t entityIndex)
		{
			if (m_ErrorMaterial == nullptr)
				return;

			Item& object = m_Buffer.emplace_back();

			if (material)
				object.Material = material->GetShader() == nullptr ? m_ErrorMaterial : material;
			else
				object.Material = m_ErrorMaterial;

			object.Flags = flags;
			object.Mesh = mesh;
			object.SubMeshIndex = subMesh;
			object.Transform = transform;

			glm::vec3 center = mesh->GetSubMeshes()[subMesh].Bounds.GetCenter();
			center = object.Transform.RotationScale * center + object.Transform.Translation;
			object.SortKey = glm::distance2(center, m_CameraPosition);
		}

		inline size_t GetSize() const { return m_Buffer.size(); }
		inline Item& operator[](size_t index) { return m_Buffer[index]; }
		inline const Item& operator[](size_t index) const { return m_Buffer[index]; }
	private:
		glm::vec3 m_CameraPosition;
		Ref<const Material> m_ErrorMaterial;
		std::vector<Item> m_Buffer;

		friend class Renderer;
	};
}
