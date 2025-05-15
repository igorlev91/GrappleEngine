#pragma once

#include "Grapple/Renderer/ShaderCacheManager.h"

#include <filesystem>

namespace Grapple
{
	class EditorShaderCache : public ShaderCacheManager
	{
	public:
		void SetCache(AssetHandle shaderHandle,
			ShaderTargetEnvironment targetEnvironment,
			ShaderStageType stageType,
			const std::vector<uint32_t>& compiledShader) override;

		std::optional<std::vector<uint32_t>> FindCache(AssetHandle shaderHandle,
			ShaderTargetEnvironment targetEnvironment,
			ShaderStageType stageType) override;

		Ref<const ShaderMetadata> FindShaderMetadata(AssetHandle shaderHandle) override;

		bool HasCache(AssetHandle shaderHandle,
			ShaderTargetEnvironment targetEnvironment,
			ShaderStageType stage) override;

		void SetShaderEntry(AssetHandle shaderHandle,
			Ref<const ShaderMetadata> metadata);

		std::filesystem::path GetCacheDirectoryPath(AssetHandle shaderHandle);
		std::string GetCacheFileName(std::string_view shaderName, ShaderTargetEnvironment targetEnvironemt, ShaderStageType stageType);
	private:
		std::unordered_map<AssetHandle, Ref<const ShaderMetadata>> m_Entries;
	};
}