#include "Asset.h"

namespace Grapple
{
	Grapple_IMPL_TYPE(AssetHandle);

	Grapple_API std::string_view AssetSourceToString(AssetSource source)
	{
		switch (source)
		{
		case AssetSource::File:
			return "File";
		case AssetSource::Memory:
			return "Memory";
		}

		Grapple_CORE_ASSERT("Unhandled asset source type");
		return "";
	}

	Grapple_API AssetSource AssetSourceFromString(std::string_view string)
	{
		if (string == "File")
			return AssetSource::File;
		if (string == "Memory")
			return AssetSource::Memory;

		Grapple_CORE_ASSERT("Unknown asset source type '{}'", string);
		return AssetSource::File;
	}

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
		case AssetType::Sprite:
			return "Sprite";
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
		case AssetType::MaterialsTable:
			return "MaterialsTable";
		}

		Grapple_CORE_ASSERT(false, "Unhandled asset type");
		return "";
	}

	AssetType AssetTypeFromString(std::string_view string)
	{
		if (string == "Texture")
			return AssetType::Texture;
		else if (string == "Sprite")
			return AssetType::Sprite;
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
		else if (string == "MaterialsTable")
			return AssetType::MaterialsTable;

		Grapple_CORE_ASSERT(false, "Unknown asset type string");
		return AssetType::None;
	}



	AssetDescriptor::AssetDescriptor(const SerializableObjectDescriptor& descriptor)
		: SerializationDescriptor(descriptor)
	{
		auto& container = GetDescriptors();
		container.Descriptors.push_back(this);
		container.SerializationDescritproToAsset.emplace(&descriptor, this);
	}

	Grapple_API AssetDescriptor::DescriptorsContainer& AssetDescriptor::GetDescriptors()
	{
		static DescriptorsContainer s_Descriptors;
		return s_Descriptors;
	}

	Grapple_API const AssetDescriptor* AssetDescriptor::FindBySerializationDescriptor(const SerializableObjectDescriptor& descriptor)
	{
		auto& container = GetDescriptors();
		auto it = container.SerializationDescritproToAsset.find(&descriptor);

		if (it != container.SerializationDescritproToAsset.end())
			return it->second;
		return nullptr;
	}
}
