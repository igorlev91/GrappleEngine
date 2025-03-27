#pragma once

#include "Grapple/Renderer/Texture.h"

#include "Grapple/AssetManager/Asset.h"

#include <filesystem>

namespace Grapple
{
	class TextureImporter
	{
	public:
		static Ref<Texture> ImportTexture(const AssetMetadata& metadata);
	};
}