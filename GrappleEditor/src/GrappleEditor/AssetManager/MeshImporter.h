#pragma once

#include "Grapple/AssetManager/Asset.h"
#include "Grapple/Renderer/Mesh.h"

namespace Grapple
{
	class MeshImporter
	{
	public:
		static Ref<Mesh> ImportMesh(const AssetMetadata& metadata);
	};
}