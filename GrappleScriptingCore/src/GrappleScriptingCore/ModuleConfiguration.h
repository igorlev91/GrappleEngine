#pragma once

#include "GrappleScriptingCore/Defines.h"
#include "GrappleScriptingCore/ScriptingType.h"
#include "GrappleScriptingCore/SystemInfo.h"

#include "GrappleScriptingCore/Bindings/ECS/World.h"

#include <vector>

namespace Grapple
{
	struct ModuleConfiguration
	{
		const std::vector<const ScriptingType*>* RegisteredTypes = nullptr;
		const std::vector<const SystemInfo*>* RegisteredSystems = nullptr;

		Bindings::WorldBindings* WorldBindings = nullptr;
	};
}