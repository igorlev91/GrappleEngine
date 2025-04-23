#pragma once

#include "GrappleCore/Core.h"

#include <filesystem>

namespace Grapple
{
	class Grapple_API Font
	{
	public:
		Font(const std::filesystem::path& path);
	};
}