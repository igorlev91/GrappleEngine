#pragma once

#include "GrappleECS/System/System.h"

namespace Grapple
{
	class GrappleECS_API SystemsManager;
	struct GrappleECS_API SystemInitializer
	{
		using CreateSystemFunction = System*(*)();

		SystemInitializer(const char* name, CreateSystemFunction createSystem);
		~SystemInitializer();

		constexpr SystemId GetId() const { return m_Id; }

		static std::vector<SystemInitializer*>& GetInitializers();

		const char* TypeName;
		CreateSystemFunction CreateSystem;
	private:
		SystemId m_Id;
		friend class SystemsManager;
	};

#define Grapple_SYSTEM static Grapple::SystemInitializer _SystemInitializer;
#define Grapple_IMPL_SYSTEM(systemName) Grapple::SystemInitializer systemName::_SystemInitializer( \
	typeid(systemName).name(),                                                                 \
	[]() -> Grapple::System* { return new systemName(); })
}