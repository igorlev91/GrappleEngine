#include "SpriteImporter.h"

#include "GrappleCore/Profiler/Profiler.h"
#include "GrappleEditor/Serialization/YAMLSerialization.h"

#include <fstream>

namespace Grapple
{
	bool SpriteImporter::SerializeSprite(const Ref<Sprite>& sprite, const std::filesystem::path& path)
	{
		Grapple_PROFILE_FUNCTION();
		YAML::Emitter emitter;
		YAMLSerializer serializer(emitter, nullptr);

		serializer.Serialize(SerializationValue(*sprite));

		std::ofstream output(path);
		if (!output.is_open())
		{
			Grapple_CORE_ERROR("Failed to serialize Sprite: {}", path.string());
			return false;
		}

		output << emitter.c_str();
		output.close();

		return true;
	}

	bool SpriteImporter::DeserializeSprite(const Ref<Sprite>& sprite, const std::filesystem::path& path)
	{
		Grapple_PROFILE_FUNCTION();
		if (!std::filesystem::exists(path))
			return false;

		YAML::Node node = YAML::LoadFile(path.generic_string());
		
		YAMLDeserializer deserializer(node);
		deserializer.Serialize(SerializationValue(*sprite));

		return true;
	}
	
	Ref<Asset> SpriteImporter::ImportSprite(const AssetMetadata& metadata)
	{
		Grapple_PROFILE_FUNCTION();
		Ref<Sprite> sprite = CreateRef<Sprite>();
		if (!DeserializeSprite(sprite, metadata.Path))
			return nullptr;

		return sprite;
	}
}
