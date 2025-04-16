#pragma once

#include "Grapple/AssetManager/AssetManager.h"

namespace Grapple
{
	class PrefabImporter
	{
	public:
		static Ref<Asset> ImportPrefab(const AssetMetadata& metadata);
	};
}