#include "AABBVisualizer.h"

#include "Grapple/Scene/Components.h"
#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Renderer/RendererPrimitives.h"

#include "Grapple/DebugRenderer/DebugRenderer.h"

#include "GrappleECS/World.h"

#include "GrappleEditor/EditorLayer.h"

namespace Grapple
{
	Grapple_IMPL_SYSTEM(AABBVisualizer);

	void AABBVisualizer::OnConfig(World& world, SystemConfig& config)
	{
		m_Query = world.NewQuery()
			.All()
			.With<TransformComponent>()
			.With<MeshComponent>()
			.Build();
		m_DecalsQuery = world.NewQuery()
			.All()
			.With<TransformComponent>()
			.With<Decal>()
			.Build();

		std::optional<uint32_t> groupId = world.GetSystemsManager().FindGroup("Debug Rendering");
		Grapple_CORE_ASSERT(groupId.has_value());
		config.Group = *groupId;
	}

	void AABBVisualizer::OnUpdate(World& world, SystemExecutionContext& context)
	{
		if (!EditorLayer::GetInstance().GetSceneViewSettings().ShowAABBs)
			return;

		for (EntityView view : m_Query)
		{
			auto transforms = view.View<TransformComponent>();
			auto meshes = view.View<MeshComponent>();

			for (EntityViewElement entity : view)
			{
				glm::mat4 transform = transforms[entity].GetTransformationMatrix();

				if (meshes[entity].Mesh == nullptr)
					continue;

				const auto& subMeshes = meshes[entity].Mesh->GetSubMeshes();

				for (const auto& subMesh : subMeshes)
				{
					const Math::AABB& bounds = subMesh.Bounds;

					glm::vec3 corners[8];
					bounds.GetCorners(corners);

					Math::AABB newBounds;
					for (size_t i = 0; i < 8; i++)
					{
						glm::vec3 transformed = transform * glm::vec4(corners[i], 1.0f);
						if (i == 0)
						{
							newBounds.Min = transformed;
							newBounds.Max = transformed;
						}
						else
						{
							newBounds.Min = glm::min(newBounds.Min, transformed);
							newBounds.Max = glm::max(newBounds.Max, transformed);
						}
					}

					DebugRenderer::DrawAABB(newBounds);
				}
			}
		}

		Math::AABB cubeAABB = RendererPrimitives::GetCube()->GetSubMeshes()[0].Bounds;
		for (EntityView view : m_DecalsQuery)
		{
			auto transforms = view.View<TransformComponent>();
			auto decals = view.View<Decal>();

			for (EntityViewElement entity : view)
			{
				DebugRenderer::DrawAABB(cubeAABB.Transformed(transforms[entity].GetTransformationMatrix()));
			}
		}
	}
}
