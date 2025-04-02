#include "ScriptingType.h"

namespace Grapple::Internal
{
	std::vector<const ScriptingType*>& ScriptingType::GetRegisteredTypes()
	{
		static std::vector<const ScriptingType*> s_RegisteredTypes;
		return s_RegisteredTypes;
	}
}