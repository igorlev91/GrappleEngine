#pragma once

#include "Grapple/Renderer/Texture.h"
#include "Grapple/AssetManager/AssetManager.h"

namespace Grapple
{
	struct TextureImportSettings
	{
		TextureImportSettings()
			: WrapMode(TextureWrap::Repeat),
			Filtering(TextureFiltering::Linear) {}

		TextureWrap WrapMode;
		TextureFiltering Filtering;
	};

	class TextureImporter
	{
	public:
		static std::filesystem::path GetImportSettingsPath(AssetHandle handle);

		static bool SerialiazeImportSettings(AssetHandle assetHandle, const TextureImportSettings& settings);
		static bool DeserializeImportSettings(AssetHandle assetHandle, TextureImportSettings& settings);

		static Ref<Asset> ImportTexture(const AssetMetadata& metadata);
	};
}