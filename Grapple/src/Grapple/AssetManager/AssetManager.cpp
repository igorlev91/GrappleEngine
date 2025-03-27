#include "AssetManager.h"

namespace Grapple
{
	Ref<AssetManagerBase> AssetManager::s_Instance = nullptr;

	void AssetManager::Intialize(const Ref<AssetManagerBase>& assetManager)
	{
		s_Instance = assetManager;
	}
}
