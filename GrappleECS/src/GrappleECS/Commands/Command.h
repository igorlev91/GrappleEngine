#pragma once

#include "GrappleECS/World.h"

#include <stdint.h>

namespace Grapple
{
	class Command
	{
	public:
		virtual void Apply(World& world) = 0;
	};

	using ApplyCommandFunction = void(*)(Command*, World&);

	struct CommandMetadata
	{
		ApplyCommandFunction Apply;
		size_t CommandSize;
	};
}
