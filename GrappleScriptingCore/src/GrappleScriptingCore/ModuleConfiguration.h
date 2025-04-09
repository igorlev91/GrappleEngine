#pragma once

#include "GrappleScriptingCore/Defines.h"
#include "GrappleScriptingCore/ScriptingType.h"

#include "GrappleScriptingCore/ECS/EntityView.h"
#include "GrappleScriptingCore/ECS/World.h"
#include "GrappleScriptingCore/ECS/ComponentInfo.h"
#include "GrappleScriptingCore/ECS/SystemInfo.h"

#include "GrappleScriptingCore/Time.h"
#include "GrappleScriptingCore/Input.h"

#include <vector>

namespace Grapple::Scripting
{
	struct ModuleConfiguration
	{
		const std::vector<ScriptingType*>* RegisteredTypes = nullptr;
		const std::vector<SystemInfo*>* RegisteredSystems = nullptr;
		const std::vector<ComponentInfo*>* RegisteredComponents = nullptr;

		Scripting::TimeData* TimeData = nullptr;
	};
}