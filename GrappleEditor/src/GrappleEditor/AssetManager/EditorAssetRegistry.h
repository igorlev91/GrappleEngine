#pragma once

#include "Grapple/AssetManager/Asset.h"
#include "Grapple/Project/Project.h"

#include <stdint.h>
#include <map>
#include <optional>

namespace Grapple
{
	struct AssetRegistryEntry
	{
		bool IsBuiltIn = false;
		AssetMetadata Metadata;
	};

	using EditorAssetRegistry = std::map<AssetHandle, AssetRegistryEntry>;

	class AssetRegistrySerializer
	{
	public:
		static void Serialize(const EditorAssetRegistry& registry, const std::filesystem::path& path);
		static bool Deserialize(EditorAssetRegistry& registry, const std::filesystem::path& path);

		inline static std::filesystem::path GetAssetsRoot()
		{
			return Project::GetActive()->Location / AssetsDirectoryName;
		}

		static const std::filesystem::path RegistryFileName;
		static const std::filesystem::path AssetsDirectoryName;
	};
}