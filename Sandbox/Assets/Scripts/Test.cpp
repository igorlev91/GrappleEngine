#include "GrappleScriptingCore/ScriptingType.h"

#include <stdint.h>

namespace Sandbox
{
	struct HealthComponent
	{
		Grapple_DEFINE_SCRIPTING_TYPE(HealthComponent);

		uint32_t Health;
		uint32_t MaxHealth;
	};
	Grapple_IMPL_SCRIPTING_TYPE(HealthComponent);
}