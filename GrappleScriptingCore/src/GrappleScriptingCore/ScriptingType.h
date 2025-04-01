#pragma once

#include "GrappleScriptingCore/Defines.h"

#include <string>
#include <vector>
#include <typeinfo>
#include <iostream>

namespace Grapple
{
	class ScriptingType
	{
	public:
		ScriptingType(const std::string& name, size_t size)
			: Name(name), Size(size)
		{
			GetRegisteredTypes().push_back(*this);
		}

		const std::string Name;
		const size_t Size;
	public:
		static std::vector<ScriptingType>& GetRegisteredTypes();
	};

	Grapple_API const std::vector<ScriptingType>& GetRegisteredScriptingTypes();
}

#define Grapple_DEFINE_SCRIPTING_TYPE(type) public: \
	static Grapple::ScriptingType GrappleScriptingType;
#define Grapple_IMPL_SCRIPTING_TYPE(type) \
	Grapple::ScriptingType type::GrappleScriptingType = Grapple::ScriptingType(typeid(type).name(), sizeof(type));
