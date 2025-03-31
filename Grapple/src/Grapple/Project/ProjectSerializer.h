#pragma once

#include "Grapple/Project/Project.h"
#include "Grapple/Serialization/Serialization.h"

namespace Grapple
{
	class ProjectSerializer
	{
	public:
		static void Serialize(const Ref<Project>& project, const std::filesystem::path& path);
		static void Deserialize(const Ref<Project>& project, const std::filesystem::path& path);
	private:
		static std::filesystem::path s_ProjectFileExtension;
	};
}