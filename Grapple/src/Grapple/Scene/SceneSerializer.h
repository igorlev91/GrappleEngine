#pragma once

#include "Grapple/Scene/Scene.h"

#include <filesystem>

namespace Grapple
{
	class Grapple_API SceneSerializer
	{
	public:
		static void Serialize(const Ref<Scene>& scene);
		static void Serialize(const Ref<Scene>& scene, const std::filesystem::path& path);
		static void Deserialize(const Ref<Scene>& scene, const std::filesystem::path& path);
	};
}