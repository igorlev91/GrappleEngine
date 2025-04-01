#include "ScriptingType.h"

namespace Grapple
{
	std::vector<ScriptingType>& ScriptingType::GetRegisteredTypes()
	{
		static std::vector<ScriptingType> s_RegisteredTypes;
		return s_RegisteredTypes;
	}

	Grapple_API const std::vector<ScriptingType>& GetRegisteredScriptingTypes()
	{
		return ScriptingType::GetRegisteredTypes();
	}
}