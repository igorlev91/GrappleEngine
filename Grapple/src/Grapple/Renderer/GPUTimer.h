#pragma once

#include "GrappleCore/Core.h"

#include <optional>

namespace Grapple
{
	class GPUTimer
	{
	public:
		virtual ~GPUTimer() {}

		virtual void Start() = 0;
		virtual void Stop() = 0;

		virtual std::optional<float> GetElapsedTime() = 0;
	public:
		static Ref<GPUTimer> Create();
	};
}
