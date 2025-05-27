#pragma once

#include "GrappleECS/Entity/Component.h"
#include "GrappleECS/Entity/ComponentInitializer.h"

#include <stdint.h>

namespace Grapple
{
	enum class QueryFilterType : uint32_t
	{
		With = 0,
		Without = 1ui32 << 31ui32,
	};
}