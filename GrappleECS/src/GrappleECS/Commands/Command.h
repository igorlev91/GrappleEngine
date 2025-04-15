#pragma once

#include <stdint.h>

namespace Grapple
{
	class GrappleECS_API World;
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
