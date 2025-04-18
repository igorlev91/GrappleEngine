#pragma once

#include "Grapple/Renderer/UniformBuffer.h"

#include "GrappleCore/Core.h"
#include "GrappleCore/Log.h"

#include <glm/glm.hpp>

namespace Grapple
{
	struct CameraData
	{
		CameraData()
			: Position(0.0f),
			Unused(0.0f),
			Projection(0.0f),
			View(0.0f),
			ViewProjection(0.0f),
			InverseProjection(0.0f),
			InverseView(0.0f),
			InverseViewProjection(0.0f) {}

		void CalculateViewProjection()
		{
			ViewProjection = Projection * View;
			InverseView = glm::inverse(View);
			InverseViewProjection = glm::inverse(ViewProjection);
		}

		glm::vec3 Position;

		float Unused; // Is needed to align the struct to 16 byte boundary

		glm::mat4 Projection;
		glm::mat4 View;
		glm::mat4 ViewProjection;

		glm::mat4 InverseProjection;
		glm::mat4 InverseView;
		glm::mat4 InverseViewProjection;
	};

	struct RenderData
	{
		CameraData Camera;
		bool IsEditorCamera = false;
	};
}