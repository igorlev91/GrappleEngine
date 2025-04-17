#pragma once

#include "Grapple/AssetManager/AssetManager.h"

#include "GrappleECS/World.h"

namespace Grapple
{
	class PrefabImporter
	{
	public:
		static void SerializePrefab(AssetHandle prefab, World& world, Entity entity);
		static Ref<Asset> ImportPrefab(const AssetMetadata& metadata);
	};
}