#pragma once

#include "GrappleScriptingCore/Defines.h"
#include "GrappleScriptingCore/ScriptingType.h"

#include "GrappleScriptingCore/Bindings/ECS/ECS.h"
#include "GrappleScriptingCore/Bindings/ECS/EntityView.h"
#include "GrappleScriptingCore/Bindings/ECS/World.h"
#include "GrappleScriptingCore/Bindings/ECS/ComponentInfo.h"
#include "GrappleScriptingCore/Bindings/ECS/SystemInfo.h"

#include "GrappleScriptingCore/Bindings/Time.h"
#include "GrappleScriptingCore/Bindings/Input.h"

#include <vector>

namespace Grapple::Internal
{
	struct ModuleConfiguration
	{
		const std::vector<ScriptingType*>* RegisteredTypes = nullptr;
		const std::vector<SystemInfo*>* RegisteredSystems = nullptr;
		const std::vector<ComponentInfo*>* RegisteredComponents = nullptr;

		Internal::WorldBindings* WorldBindings = nullptr;
		Internal::EntityViewBindings* EntityViewBindings = nullptr;
		Internal::TimeData* TimeData = nullptr;
		Internal::InputBindings* InputBindings = nullptr;
	};
}