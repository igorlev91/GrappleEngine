#pragma once

#include "Grapple/Core/UUID.h"

#include "GrappleECS/ComponentId.h"
#include "GrappleECS/EntityId.h"

#include "GrappleScriptingCore/ECS/World.h"
#include "GrappleScriptingCore/ECS/EntityView.h"
#include "GrappleScriptingCore/ECS/SystemInfo.h"
#include "GrappleScriptingCore/ECS/SystemConfiguration.h"
#include "GrappleScriptingCore/ECS/ComponentInfo.h"

#include "GrappleScriptingCore/Time.h"
#include "GrappleScriptingCore/Input.h"

#include "GrappleScriptingCore/Texture.h"

#include "GrappleScriptingCore/ScriptingType.h"

#include "GrappleScriptingCore/TypeSerializationSettings.h"

namespace Grapple
{
	using Scripting::World;
	using Scripting::EntityView;
	using Scripting::EntityElement;
	using Scripting::ComponentView;

	using Scripting::ComponentInfo;
	using Scripting::ScriptingType;
	using Scripting::SystemBase;
	using Scripting::SystemInfo;
	using Scripting::SystemConfiguration;

	using Scripting::Time;
	using Scripting::Input;

	using Scripting::TextureAsset;

	using Scripting::TypeSerializationSettings;

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
