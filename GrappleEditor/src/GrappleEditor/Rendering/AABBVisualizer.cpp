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

		m_Query.ForEachChunk([](QueryChunk chunk,
			ComponentView<const MeshComponent> meshes,
			ComponentView<const TransformComponent> transforms)
			{
				for (auto entity : chunk)
				{
					glm::mat4 transform = transforms[entity].GetTransformationMatrix();

					if (meshes[entity].Mesh == nullptr)
						continue;

					Math::AABB meshBounds = meshes[entity].Mesh->GetBounds();
					DebugRenderer::DrawAABB(meshBounds.Transformed(transform));
				}
			});

		Math::AABB cubeAABB = RendererPrimitives::GetCube()->GetBounds();
		m_DecalsQuery.ForEachChunk([&cubeAABB](QueryChunk chunk,
			ComponentView<const TransformComponent> transforms,
			ComponentView<const Decal> decals)
			{
				for (auto entity : chunk)
				{
					DebugRenderer::DrawAABB(cubeAABB.Transformed(transforms[entity].GetTransformationMatrix()));
				}
			});
	}
}
