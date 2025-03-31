#pragma once

#include "Grapple/Renderer/CameraData.h"

namespace Grapple
{
	struct RenderData
	{
		CameraData Camera;
		glm::u32vec2 ViewportSize = glm::u32vec2(0);
		bool IsEditorCamera = false;
	};
}