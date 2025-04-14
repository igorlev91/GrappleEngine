#pragma once

#include "Grapple/Core/Core.h"
#include "Grapple/AssetManager/AssetManagerBase.h"

namespace Grapple
{
	class Grapple_API AssetManager
	{
	public:
		static void Intialize(const Ref<AssetManagerBase>& assetManager);

		static Ref<AssetManagerBase> GetInstance();
	public:
		template<typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			return As<T>(GetRawAsset(handle));
		}

		static const AssetMetadata* GetAssetMetadata(AssetHandle handle);
		static Ref<Asset> GetRawAsset(AssetHandle handle);
		static bool IsAssetHandleValid(AssetHandle handle);
		static bool IsAssetLoaded(AssetHandle handle);
	};
}