#pragma once

#include "Grapple/AssetManager/Asset.h"
#include "GrappleCore/Serialization/Metadata.h"

namespace Grapple
{
	class Grapple_API MaterialsTable : public Asset
	{
	public:
		Grapple_ASSET;
		Grapple_SERIALIZABLE;

		MaterialsTable();
	public:
		std::vector<AssetHandle> Materials;
	};
}
