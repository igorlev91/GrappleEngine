#pragma once

#include "Grapple/AssetManager/Asset.h"

#include <vector>
#include <filesystem>

namespace Grapple
{
	class ShaderCompiler
	{
	public:
		static bool Compile(AssetHandle shaderHandle);
	};
}