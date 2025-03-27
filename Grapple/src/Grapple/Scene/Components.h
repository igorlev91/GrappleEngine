#pragma once

#include "Grapple.h"

#include "GrappleECS/World.h"

namespace Grapple
{
	struct TransformComponent
	{
		Grapple_COMPONENT;

		glm::vec3 Position;
		glm::vec3 Rotation;
		glm::vec3 Scale;

		glm::mat4 GetTransformationMatrix() const;
	};

	struct CameraComponent
	{
		Grapple_COMPONENT;

		float Size;
		float Near;
		float Far;
	};

	struct SpriteComponent
	{
		Grapple_COMPONENT;

		glm::vec4 Color;
	};
}