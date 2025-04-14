#pragma once

#include "Grapple/Core/Core.h"

#include "Grapple/Scene/Scene.h"
#include "Grapple/AssetManager/Asset.h"

namespace Grapple
{
	class Grapple_API SceneImporter
	{
	public:
		static Ref<Scene> ImportScene(const AssetMetadata& metadata);
	};
}