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

				if (camera.Projection == CameraComponent::ProjectionType::Perspective)
				{
					float halfFOV = glm::radians(camera.FOV / 2.0f);
					glm::vec2 forwardAndUpOffset = glm::vec2(glm::cos(halfFOV), glm::sin(halfFOV));
					float rightOffset = forwardAndUpOffset.y * viewportAspectRatio;

					glm::vec3 localTopRight = glm::vec3(rightOffset, forwardAndUpOffset.y, -forwardAndUpOffset.x);
					glm::quat rotation = glm::quat(glm::radians(transform.Rotation));
					for (uint32_t i = 0; i < 4; i++)
					{
						nearPoints[i] = transform.Position + glm::rotate(rotation, localTopRight * camera.Near * offsetSigns[i]);
						farPoints[i] = transform.Position + glm::rotate(rotation, localTopRight * camera.Far * offsetSigns[i]);
					}
				}
				else
				{
					glm::vec2 offset = glm::vec2(camera.Size / 2.0f * viewportAspectRatio, camera.Size / 2.0f);
					glm::quat rotation = glm::quat(glm::radians(transform.Rotation));
					for (uint32_t i = 0; i < 4; i++)
					{
						nearPoints[i] = transform.Position + glm::rotate(rotation, glm::vec3(offset, camera.Near) * offsetSigns[i]);
						farPoints[i] = transform.Position + glm::rotate(rotation, glm::vec3(offset, -camera.Far) * offsetSigns[i]);
					}
				}

				for (uint32_t i = 0; i < 4; i++)
				{
					DebugRenderer::DrawLine(nearPoints[i], farPoints[i]);
					DebugRenderer::DrawLine(nearPoints[i], nearPoints[(i + 1) % 4]);
					DebugRenderer::DrawLine(farPoints[i], farPoints[(i + 1) % 4]);
				}
			}
		}
	}
}
