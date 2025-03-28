#pragma once

#include "Grapple.h"

#include "GrappleECS/World.h"

#include "Grapple/AssetManager/Asset.h"

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

		enum class ProjectionType : uint8_t
		{
			Orthographic,
			Perspective,
		};

		ProjectionType Projection;

		float Size;
		float FOV;
		float Near;
		float Far;
	};

	struct SpriteComponent
	{
		Grapple_COMPONENT;

		glm::vec4 Color;
		AssetHandle Texture;
	};
}