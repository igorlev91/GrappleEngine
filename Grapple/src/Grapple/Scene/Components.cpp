#include "Components.h"

namespace Grapple
{
	Grapple_COMPONENT_IMPL(TransformComponent);
	Grapple_COMPONENT_IMPL(CameraComponent);
	Grapple_COMPONENT_IMPL(SpriteComponent);

	glm::mat4 TransformComponent::GetTransformationMatrix() const
	{
		return glm::translate(glm::mat4(1.0f), Position) * glm::toMat4(glm::quat(glm::radians(Rotation))) *
			glm::scale(glm::mat4(1.0f), Scale);
	}
}