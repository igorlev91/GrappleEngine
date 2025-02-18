#pragma once

#include <Grapple/Core/Core.h>

namespace Grapple
{
	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void Initialize() = 0;
		virtual void SwapBuffers() = 0;
	public:
		static Scope<GraphicsContext> Create(void* windowHandle);
	};
}