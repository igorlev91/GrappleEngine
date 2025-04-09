#pragma once

#include "GrappleScriptingCore/ScriptingType.h"
#include "GrappleScriptingCore/Bindings/ECS/SystemConfiguration.h"
#include "GrappleScriptingCore/Bindings/ECS/EntityView.h"

#include <string>
#include <string_view>
#include <typeinfo>

namespace Grapple::Internal
{
	struct SystemInfo
	{
		SystemInfo(const std::string_view& name)
			: Name(name)
		{
			GetRegisteredSystems().push_back(this);
		}

		const std::string_view Name;
	public:
		static std::vector<const SystemInfo*>& GetRegisteredSystems();
	};

	class SystemBase
	{
	public:
		virtual void Configure(SystemConfiguration& config) = 0;
		virtual void Execute(Internal::EntityView& chunk) = 0;
	};

#ifndef Grapple_SCRIPTING_CORE_NO_MACROS
	#define Grapple_SYSTEM(systemName) \
			Grapple_DEFINE_SCRIPTING_TYPE(systemName) \
			static Grapple::SystemInfo System;

	#define Grapple_SYSTEM_IMPL(systemName) \
		Grapple_IMPL_SCRIPTING_TYPE(systemName, systemName::ConfigureSerialization) \
		Grapple::SystemInfo systemName::System = Grapple::SystemInfo(typeid(systemName).name());
#endif
}