#pragma once

#include "Grapple/Scene/Components.h"

#include "GrappleECS/World.h"

namespace Grapple
{
	class EntityProperties
	{
	public:
		EntityProperties(World& world);

		void OnRenderImGui(Entity entity);
	private:
		void EntityProperties::RenderCameraComponent(CameraComponent& cameraComponent);
		void EntityProperties::RenderTransformComponent(TransformComponent& transform);
		void EntityProperties::RenderSpriteComponent(SpriteComponent& sprite);

		void EntityProperties::RenderAddComponentMenu(Entity entity);
	private:
		World& m_World;
	};
}