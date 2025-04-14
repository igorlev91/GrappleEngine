#pragma once

#include "GrappleECS/System/System.h"

namespace Grapple
{
	struct GrappleECS_API SystemInitializer
	{
		using CreateSystemFunction = System*(*)();

		SystemInitializer(const char* name, CreateSystemFunction createSystem);
		~SystemInitializer();

		static std::vector<SystemInitializer*>& GetInitializers();

		const char* TypeName;
		CreateSystemFunction CreateSystem;
	};

#define Grapple_SYSTEM static Grapple::SystemInitializer _SystemInitializer;
#define Grapple_IMPL_SYSTEM(systemName) Grapple::SystemInitializer systemName::_SystemInitializer( \
	typeid(systemName).name(),                                                                 \
	[]() -> Grapple::System* { return new systemName(); })
}