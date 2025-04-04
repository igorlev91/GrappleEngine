#pragma once

#include <stdint.h>

namespace Grapple::Internal
{
	using ComponentId = uint32_t;
	constexpr ComponentId INVALID_COMPONENT_ID = UINT32_MAX;
}
