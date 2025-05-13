#pragma once

#include "Grapple/AssetManager/Asset.h"

namespace Grapple
{
	class Grapple_API MaterialsTable : public Asset
	{
	public:
		MaterialsTable();
	public:
		std::vector<AssetHandle> Materials;
	};
}
