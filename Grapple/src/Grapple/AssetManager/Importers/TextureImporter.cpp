#include "TextureImporter.h"

namespace Grapple
{
	Ref<Texture> TextureImporter::ImportTexture(const AssetMetadata& metadata)
	{
		return Texture::Create(metadata.Path);
	}
}
