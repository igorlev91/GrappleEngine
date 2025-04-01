#pragma once

#include "GrappleScriptingCore/Defines.h"
#include "GrappleScriptingCore/ScriptingType.h"

#include <vector>

namespace Grapple
{
	struct ModuleConfiguration
	{
		const std::vector<ScriptingType>* RegisteredTypes;
	};
}