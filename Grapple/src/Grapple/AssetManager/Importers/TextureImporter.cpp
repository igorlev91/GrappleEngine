#include "TextureImporter.h"

namespace Grapple
{
	Ref<Texture> TextureImporter::ImportTexture(const AssetMetadata& metadata)
	{
		// NOTE: TextureFiltering::Closest is default, util asset import settings are implemented
		return Texture::Create(metadata.Path, TextureFiltering::Closest);
	}
}
