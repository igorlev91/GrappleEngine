#include "ScriptingType.h"

namespace Grapple::Scripting
{
	std::vector<ScriptingType*>& ScriptingType::GetRegisteredTypes()
	{
		static std::vector<ScriptingType*> s_RegisteredTypes;
		return s_RegisteredTypes;
	}
}