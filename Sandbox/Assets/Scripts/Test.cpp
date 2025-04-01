#include "GrappleScriptingCore/ScriptingType.h"
#include "GrappleScriptingCore/SystemInfo.h"

#include "Grapple/Core/Log.h"

#include <stdint.h>
#include <iostream>

namespace Sandbox
{
	struct TestSystem : public Grapple::SystemBase
	{
		Grapple_SYSTEM(TestSystem);

		virtual void Configure(Grapple::SystemConfiguration& config) override
		{
		}

		virtual void Execute() override
		{
		}
	};

	Grapple_SYSTEM_IMPL(TestSystem);
}