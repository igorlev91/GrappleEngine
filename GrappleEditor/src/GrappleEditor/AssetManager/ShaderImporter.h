#pragma once

#include "Grapple/AssetManager/Asset.h"
#include "Grapple/Renderer/Shader.h"

namespace Grapple
{
	class ShaderImporter
	{
	public:
		static Ref<Asset> ImportShader(const AssetMetadata& metadata);
	};
}