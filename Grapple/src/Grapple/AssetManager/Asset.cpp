#include "Asset.h"

namespace Grapple
{
	Grapple_IMPL_TYPE(AssetHandle);

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
		case AssetType::Prefab:
			return "Prefab";
		case AssetType::Shader:
			return "Shader";
		case AssetType::Material:
			return "Material";
		case AssetType::Font:
			return "Font";
		case AssetType::Mesh:
			return "Mesh";
		}

		Grapple_CORE_ASSERT(false, "Unhandled asset type");
		return "";
	}

	AssetType AssetTypeFromString(std::string_view string)
	{
		if (string == "Texture")
			return AssetType::Texture;
		else if (string == "Scene")
			return AssetType::Scene;
		else if (string == "Prefab")
			return AssetType::Prefab;
		else if (string == "Shader")
			return AssetType::Shader;
		else if (string == "Material")
			return AssetType::Material;
		else if (string == "Font")
			return AssetType::Font;
		else if (string == "Mesh")
			return AssetType::Mesh;

		Grapple_CORE_ASSERT(false, "Unknown asset type string");
		return AssetType::None;
	}
}
