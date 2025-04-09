#pragma once

#include "GrappleECS/System.h"

#include "GrappleScriptingCore/ScriptingType.h"
#include "GrappleScriptingCore/ECS/SystemConfiguration.h"
#include "GrappleScriptingCore/ECS/EntityView.h"

#include <string>
#include <string_view>
#include <typeinfo>

namespace Grapple::Scripting
{
	struct SystemInfo
	{
		SystemInfo(const std::string_view& name)
			: Name(name), Id(UINT32_MAX)
		{
			GetRegisteredSystems().push_back(this);
		}

		SystemId Id;
		const std::string_view Name;
	public:
		static std::vector<SystemInfo*>& GetRegisteredSystems();
	};

	class SystemBase
	{
	public:
		virtual void Configure(SystemConfiguration& config) = 0;
		virtual void Execute(Scripting::EntityView& chunk) = 0;
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