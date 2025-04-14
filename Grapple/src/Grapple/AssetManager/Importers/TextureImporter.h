#pragma once

#include "Grapple/Core/Core.h"

#include "Grapple/Renderer/Texture.h"
#include "Grapple/AssetManager/Asset.h"

#include <filesystem>

namespace Grapple
{
	class Grapple_API TextureImporter
	{
	public:
		static Ref<Texture> ImportTexture(const AssetMetadata& metadata);
	};
}