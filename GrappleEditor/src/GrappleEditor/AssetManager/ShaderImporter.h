#pragma once

#include "Grapple/AssetManager/Asset.h"

namespace Grapple
{
	class ShaderImporter
	{
	public:
		static Ref<Asset> ImportShader(const AssetMetadata& metadata);
		static Ref<Asset> ImportComputeShader(const AssetMetadata& metadata);
	};
}