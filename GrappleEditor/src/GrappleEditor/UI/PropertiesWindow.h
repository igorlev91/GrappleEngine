#pragma once

#include "Grapple/Scene/Components.h"

#include "GrappleScriptingCore/ScriptingType.h"
#include "GrappleScriptingCore/TypeSerializationSettings.h"

#include <unordered_map>
#include <functional>

namespace Grapple
{
	class PropertiesWindow
	{
	public:
		void OnImGuiRender();
	private:
		void RenderAddComponentMenu(Entity entity);

		void RenderCameraComponent(CameraComponent& cameraComponent);
		void RenderTransformComponent(TransformComponent& transform);
		void RenderSpriteComponent(SpriteComponent& sprite);
	};
}