#include "SceneImporter.h"

#include "Grapple/Scene/SceneSerializer.h"

namespace Grapple
{
	Ref<Scene> SceneImporter::ImportScene(const AssetMetadata& metadata)
	{
		Ref<Scene> scene = CreateRef<Scene>();
		SceneSerializer::Deserialize(scene, metadata.Path);
		return scene;
	}
}
