#pragma once

#include "Grapple.h"

#include "GrappleECS/World.h"
#include "GrappleECS/Entity/ComponentInitializer.h"

#include "Grapple/AssetManager/Asset.h"

namespace Grapple
{
	struct Grapple_API TransformComponent
	{
		Grapple_COMPONENT;

		TransformComponent();

		glm::vec3 Position;
		glm::vec3 Rotation;
		glm::vec3 Scale;

		glm::mat4 GetTransformationMatrix() const;
	};

	struct Grapple_API CameraComponent
	{
		Grapple_COMPONENT;

		enum class ProjectionType : uint8_t
		{
			Orthographic,
			Perspective,
		};

		CameraComponent();

		glm::mat4 GetProjection() const;
		glm::vec3 ScreenToWorld(glm::vec2 point) const;
		glm::vec3 ViewportToWorld(glm::vec2 point) const;

		ProjectionType Projection;

		float Size;
		float FOV;
		float Near;
		float Far;
	};

	struct Grapple_API SpriteComponent
	{
		Grapple_COMPONENT;

		SpriteComponent();

		glm::vec4 Color;
		glm::vec2 TextureTiling;
		AssetHandle Texture;
	};
}