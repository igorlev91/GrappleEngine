#pragma once

#include "GrappleCore/Core.h"

#include "Grapple/AssetManager/Asset.h"

#include <unordered_map>

namespace Grapple
{
	class AssetRegistry
	{
	private:
		std::unordered_map<AssetHandle, Ref<Asset>> m_LaodedAssets;
	};
}