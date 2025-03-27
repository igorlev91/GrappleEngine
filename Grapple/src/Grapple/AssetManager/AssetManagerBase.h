#pragma once

#include "Grapple/Core/Core.h"
#include "Grapple/AssetManager/Asset.h"

namespace Grapple
{
	class AssetManagerBase
	{
	public:
		virtual Ref<Asset> GetAsset(AssetHandle handle) = 0;

		virtual bool IsAssetHandleValid(AssetHandle handle) = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) = 0;
	};
}