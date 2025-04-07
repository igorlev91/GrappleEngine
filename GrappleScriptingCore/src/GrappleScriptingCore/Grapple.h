#pragma once

#include "Grapple/Core/UUID.h"

#include "GrappleECS/ComponentId.h"

#include "GrappleScriptingCore/Bindings/ECS/World.h"
#include "GrappleScriptingCore/Bindings/ECS/EntityView.h"
#include "GrappleScriptingCore/Bindings/ECS/SystemInfo.h"
#include "GrappleScriptingCore/Bindings/ECS/SystemConfiguration.h"
#include "GrappleScriptingCore/Bindings/ECS/ComponentInfo.h"

#include "GrappleScriptingCore/Bindings/Time.h"
#include "GrappleScriptingCore/Bindings/Input.h"

#include "GrappleScriptingCore/Bindings/Texture.h"

#include "GrappleScriptingCore/ScriptingType.h"

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

	using Internal::TextureAsset;

	using Internal::TypeSerializationSettings;

	struct Transform
	{
		Grapple_COMPONENT(Transform);

		glm::vec3 Position;
		glm::vec3 Rotation;
		glm::vec3 Scale;
	};

	struct Sprite
	{
		Grapple_COMPONENT(Sprite);

		glm::vec4 Color;
		glm::vec2 TextureTiling;

		TextureAsset Texture;
	};
}
