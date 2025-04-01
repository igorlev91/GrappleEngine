#pragma once

#include <stdint.h>

namespace Grapple::Bindings
{
	using ComponentId = size_t;
	constexpr ComponentId INVALID_COMPONENT_ID = SIZE_MAX;
}

#ifndef Grapple_SCRIPTING_CORE_NO_MACROS
	#define Grapple_COMPONENT static Grapple::ComponentId Id;
	#define Grapple_COMPONENT_IMPL(component) Grapple::ComponentId components::Id = Grapple::INVALID_COMPONENT_ID;
#endif
