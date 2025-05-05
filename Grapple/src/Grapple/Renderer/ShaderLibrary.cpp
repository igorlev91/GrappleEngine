#include "ShaderLibrary.h"

namespace Grapple
{
	std::unordered_map<std::string, AssetHandle> s_ShaderNameToHandle;

	void ShaderLibrary::AddShader(const std::string_view& name, AssetHandle handle)
	{
		Grapple_CORE_INFO(name);
		s_ShaderNameToHandle.emplace(name, handle);
	}

	void ShaderLibrary::Clear()
	{
		s_ShaderNameToHandle.clear();
	}

	std::optional<AssetHandle> ShaderLibrary::FindShader(std::string_view name)
	{
		auto it = s_ShaderNameToHandle.find(std::string(name));
		if (it == s_ShaderNameToHandle.end())
			return {};
		return it->second;
	}
}
