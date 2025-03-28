#pragma once

#include "Grapple/Scene/Scene.h"
#include "Grapple/AssetManager/Asset.h"

namespace Grapple
{
	class SceneImporter
	{
	public:
		static Ref<Scene> ImportScene(const AssetMetadata& metadata);
	};
}