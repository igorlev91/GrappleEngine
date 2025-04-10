#pragma once

#include "GrappleECS/System/SystemData.h"

namespace Grapple
{
	class System
	{
	public:
		virtual void OnUpdate(SystemExecutionContext& context) = 0;
	};
}