#include "ShaderLibrary.h"

#include "GrappleCore/Profiler/Profiler.h"

#include "Grapple/AssetManager/AssetManager.h"

namespace Grapple
{
	std::unordered_map<std::string, AssetHandle> s_ShaderNameToHandle;

	void ShaderLibrary::AddShader(const std::string_view& name, AssetHandle handle)
	{
		s_ShaderNameToHandle.emplace(name, handle);
	}

	void ShaderLibrary::AddShader(AssetHandle handle)
	{
		Grapple_PROFILE_FUNCTION();

		Grapple_CORE_ASSERT(AssetManager::IsAssetHandleValid(handle));
		const AssetMetadata* metadata = AssetManager::GetAssetMetadata(handle);
		Grapple_CORE_ASSERT(metadata);
		Grapple_CORE_ASSERT(metadata->Type == AssetType::Shader || metadata->Type == AssetType::ComputeShader);
		Grapple_CORE_ASSERT(metadata->Source == AssetSource::File);

		std::string shaderFileName = metadata->Path.filename().string();
		size_t dotPosition = shaderFileName.find_first_of(".");

		std::string_view shaderName = shaderFileName;
		if (dotPosition != std::string_view::npos)
			shaderName = std::string_view(shaderFileName).substr(0, dotPosition);

		AddShader(shaderName, metadata->Handle);
	}

	void ShaderLibrary::Remove(AssetHandle handle)
	{
		Grapple_PROFILE_FUNCTION();
		auto it = std::find_if(
			s_ShaderNameToHandle.begin(),
			s_ShaderNameToHandle.end(),
			[handle](const std::pair<std::string, AssetHandle>& keyValue) -> bool
			{
				return handle == keyValue.second;
			});

		if (it != s_ShaderNameToHandle.end())
		{
			s_ShaderNameToHandle.erase(it);
		}
	}

	void ShaderLibrary::Clear()
	{
		s_ShaderNameToHandle.clear();
	}

	const std::unordered_map<std::string, AssetHandle>& ShaderLibrary::GetNameToHandleMap()
	{
		return s_ShaderNameToHandle;
	}

	std::optional<AssetHandle> ShaderLibrary::FindShader(std::string_view name)
	{
		auto it = s_ShaderNameToHandle.find(std::string(name));
		if (it == s_ShaderNameToHandle.end())
			return {};
		return it->second;
	}
}
