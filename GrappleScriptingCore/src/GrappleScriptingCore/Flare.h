#pragma once

#include "GrappleScriptingCore/Bindings/ECS/Component.h"
#include "GrappleScriptingCore/Bindings/ECS/World.h"
#include "GrappleScriptingCore/Bindings/ECS/EntityView.h"

#include "GrappleScriptingCore/Bindings/Time.h"

#include "GrappleScriptingCore/ComponentInfo.h"
#include "GrappleScriptingCore/SystemInfo.h"
#include "GrappleScriptingCore/ScriptingType.h"
#include "GrappleScriptingCore/SystemConfiguration.h"

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
}
