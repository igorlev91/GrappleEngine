#pragma once

#include <stdint.h>

namespace Grapple
{
	struct Grapple_API CommandLineArguments
	{
		const char** Arguments = nullptr;
		uint32_t ArgumentsCount = 0;

		const char* operator[](uint32_t index)
		{
			return Arguments[index];
		}
	};
}