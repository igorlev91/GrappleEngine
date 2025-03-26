#pragma once

#include "Grapple/Scene/Components.h"

#include <unordered_map>
#include <functional>

namespace Grapple
{
	class PropertiesWindow
	{
	public:
		void OnImGuiRender();
	private:
		void RenderCameraComponent(CameraComponent& cameraComponent);
	};
}