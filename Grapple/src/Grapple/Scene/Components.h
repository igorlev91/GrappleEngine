#pragma once

#include "GrappleECS/World.h"

#include <glm/glm.hpp>

namespace Grapple
{
	struct TransformComponent
	{
		Grapple_COMPONENT;

		glm::vec3 Position;
		glm::vec3 Rotation;
		glm::vec3 Scale;
	};

	struct CameraComponent
	{
		Grapple_COMPONENT;

		float Size;
		float Near;
		float Far;
	};
}