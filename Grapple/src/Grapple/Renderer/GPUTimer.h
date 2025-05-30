#pragma once

#include "GrappleCore/Core.h"

#include <optional>

namespace Grapple
{
	class GPUTimer
	{
	public:
		virtual ~GPUTimer() {}

		virtual std::optional<float> GetElapsedTime() = 0;
	public:
		static Ref<GPUTimer> Create();
	};
}
