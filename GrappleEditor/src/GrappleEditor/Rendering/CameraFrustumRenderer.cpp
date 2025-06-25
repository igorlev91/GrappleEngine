#include "CameraFrustumRenderer.h"

#include "GrappleCore/Profiler/Profiler.h"

#include "Grapple/Scene/Components.h"
#include "Grapple/Scene/Transform.h"

#include "Grapple/DebugRenderer/DebugRenderer.h"

#include "GrappleECS/World.h"

#include "GrappleEditor/EditorLayer.h"

namespace Grapple
{
	Grapple_IMPL_SYSTEM(CameraFrustumRenderer);

	void CameraFrustumRenderer::OnConfig(World& world, SystemConfig& config)
	{
		Grapple_PROFILE_FUNCTION();
		std::optional<uint32_t> groupId = world.GetSystemsManager().FindGroup("Debug Rendering");
		Grapple_CORE_ASSERT(groupId.has_value());
		config.Group = *groupId;

		m_Query = world.NewQuery().All().With<CameraComponent, TransformComponent>().Build();
	}

	void CameraFrustumRenderer::OnUpdate(World& world, SystemExecutionContext& context)
	{
		Grapple_PROFILE_FUNCTION();
		if (!EditorLayer::GetInstance().GetSceneViewSettings().ShowCameraFrustum)
			return;

		glm::uvec2 viewportSize = Renderer::GetMainViewport().GetSize();
		float viewportAspectRatio = (float)viewportSize.x / (float)viewportSize.y;

		glm::vec3 offsetSigns[4] = {
			glm::vec3( 1.0f,  1.0f,  1.0f),
			glm::vec3(-1.0f,  1.0f,  1.0f),
			glm::vec3(-1.0f, -1.0f,  1.0f),
			glm::vec3( 1.0f, -1.0f,  1.0f),
		};

		glm::vec3 farPoints[4];
		glm::vec3 nearPoints[4];
		for (EntityView chunk : m_Query)
		{
			auto transforms = chunk.View<TransformComponent>();
			auto cameras = chunk.View<CameraComponent>();

			for (EntityViewElement entity : chunk)
			{
				TransformComponent& transform = transforms[entity];
				CameraComponent& camera = cameras[entity];

				glm::mat4 transformaMatrix = transform.GetTransformationMatrix();
				glm::mat4 projectionMatrix = camera.GetProjection();

				glm::mat4 viewProjection = projectionMatrix * glm::inverse(transformaMatrix);
				DebugRenderer::DrawFrustum(glm::inverse(viewProjection), glm::vec4(1.0f));
			}
		}
	}
}
