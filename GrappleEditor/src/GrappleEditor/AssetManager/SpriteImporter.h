#pragma once

#include "Grapple/Renderer/Sprite.h"

#include <filesystem>

namespace Grapple
{
	class SpriteImporter
	{
	public:
		static bool SerializeSprite(const Ref<Sprite>& sprite, const std::filesystem::path& path);
		static bool DeserializeSprite(const Ref<Sprite>& sprite, const std::filesystem::path& path);

		static Ref<Asset> ImportSprite(const AssetMetadata& metadata);
	};
}