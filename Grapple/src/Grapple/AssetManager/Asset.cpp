#include "Asset.h"

namespace Grapple
{
	std::string_view AssetTypeToString(AssetType type)
	{
		switch (type)
		{
		case AssetType::None:
			return "None";
		case AssetType::Scene:
			return "Scene";
		case AssetType::Texture:
			return "Texture";
		}

		Grapple_CORE_ASSERT(false, "Unhandled asset type");
		return "";
	}
}
