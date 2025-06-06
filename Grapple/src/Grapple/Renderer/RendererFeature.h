#pragma once

#include "GrappleCore/Core.h"

namespace Grapple
{
	class Grapple_API RendererFeature
	{
	public:
		virtual ~RendererFeature() = default;

		virtual void OnUpdate() = 0;
		virtual void AddPasses() = 0;
	};
}
