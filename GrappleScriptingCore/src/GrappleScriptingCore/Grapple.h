#pragma once

#include "Grapple/Core/UUID.h"

#include "GrappleECS/ComponentId.h"

#include "GrappleScriptingCore/Bindings/ECS/World.h"
#include "GrappleScriptingCore/Bindings/ECS/EntityView.h"

#include "GrappleScriptingCore/Bindings/Time.h"
#include "GrappleScriptingCore/Bindings/Input.h"

#include "GrappleScriptingCore/ComponentInfo.h"
#include "GrappleScriptingCore/SystemInfo.h"
#include "GrappleScriptingCore/ScriptingType.h"
#include "GrappleScriptingCore/SystemConfiguration.h"

#include "GrappleScriptingCore/TypeSerializationSettings.h"

namespace Grapple
{
	using Internal::World;
	using Internal::Entity;
	using Internal::EntityView;
	using Internal::EntityElement;
	using Internal::ComponentView;

	using Internal::ComponentInfo;
	using Internal::ScriptingType;
	using Internal::SystemBase;
	using Internal::SystemInfo;
	using Internal::SystemConfiguration;

	using Internal::Time;
	using Internal::Input;

	using Internal::TypeSerializationSettings;

	struct Transform
	{
		Grapple_COMPONENT(Transform);

		glm::vec3 Position;
		glm::vec3 Rotation;
		glm::vec3 Scale;
	};
	Grapple_COMPONENT_ALIAS_IMPL(Transform, "struct Grapple::TransformComponent");

	struct Sprite
	{
		Grapple_COMPONENT(Sprite);

		glm::vec4 Color;
		glm::vec2 TextureTiling;

		UUID Texture;
	};
	Grapple_COMPONENT_ALIAS_IMPL(Sprite, "struct Grapple::SpriteComponent");
}
