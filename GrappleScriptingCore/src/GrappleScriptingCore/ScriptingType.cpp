#include "ScriptingType.h"

namespace Grapple::Internal
{
	std::vector<ScriptingType*>& ScriptingType::GetRegisteredTypes()
	{
		static std::vector<ScriptingType*> s_RegisteredTypes;
		return s_RegisteredTypes;
	}
}