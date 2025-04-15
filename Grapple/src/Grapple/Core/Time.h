#pragma once

#include "GrappleCore/Core.h"

namespace Grapple
{
	class Grapple_API Application;
	class Time
	{
	public:
		Grapple_API static float GetDeltaTime();
	private:
		static void UpdateDeltaTime();

		friend class Application;
	};
}