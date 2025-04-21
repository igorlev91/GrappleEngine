#pragma once

#include "Grapple/AssetManager/Asset.h"
#include "Grapple/Renderer/Material.h"

namespace Grapple
{
	class MaterialImporter
	{
	public:
		static void SerializeMaterial(Ref<Material> material, const std::filesystem::path& path);

		static Ref<Material> ImportMaterial(const AssetMetadata& metadata);
	};
}