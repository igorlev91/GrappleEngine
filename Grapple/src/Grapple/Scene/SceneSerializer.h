#pragma once

#include "Grapple/Scene/Scene.h"

#include <filesystem>

namespace Grapple
{
	class SceneSerializer
	{
	public:
		static void Serialize(const Ref<Scene>& scene);
		static void Deserialize(const Ref<Scene>& scene, const std::filesystem::path& path);
	};
}