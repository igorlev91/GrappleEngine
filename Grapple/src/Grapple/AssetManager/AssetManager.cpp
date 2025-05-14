#include "AssetManager.h"

#include "GrappleCore/Profiler/Profiler.h"

namespace Grapple
{
	Ref<AssetManagerBase> s_Instance = nullptr;

	void AssetManager::Intialize(const Ref<AssetManagerBase>& assetManager)
	{
		s_Instance = assetManager;
	}

	Ref<AssetManagerBase> AssetManager::GetInstance()
	{
		return s_Instance;
	}

	const AssetMetadata* AssetManager::GetAssetMetadata(AssetHandle handle)
	{
		Grapple_PROFILE_FUNCTION();
		return s_Instance->GetAssetMetadata(handle);
	}

	Ref<Asset> AssetManager::GetRawAsset(AssetHandle handle)
	{
		Grapple_PROFILE_FUNCTION();
		return s_Instance->GetAsset(handle);
	}

	bool AssetManager::IsAssetHandleValid(AssetHandle handle)
	{
		Grapple_PROFILE_FUNCTION();
		return s_Instance->IsAssetHandleValid(handle);
	}

	bool AssetManager::IsAssetLoaded(AssetHandle handle)
	{
		Grapple_PROFILE_FUNCTION();
		return s_Instance->IsAssetLoaded(handle);
	}
}
