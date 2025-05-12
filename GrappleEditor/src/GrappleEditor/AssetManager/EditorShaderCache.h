#pragma once

#include "Grapple/Renderer/ShaderCacheManager.h"

#include <filesystem>

namespace Grapple
{
	class EditorShaderCache : public ShaderCacheManager
	{
	public:
		struct ShaderEntry
		{
			ShaderEntry() = default;
			ShaderEntry(ShaderFeatures features, ShaderProperties&& properties)
				: Features(features), Properties(std::move(properties)) {}

			ShaderFeatures Features;
			ShaderProperties Properties;
		};

		void SetCache(AssetHandle shaderHandle,
			ShaderTargetEnvironment targetEnvironment,
			ShaderStageType stageType,
			const std::vector<uint32_t>& compiledShader) override;

		std::optional<std::vector<uint32_t>> FindCache(AssetHandle shaderHandle,
			ShaderTargetEnvironment targetEnvironment,
			ShaderStageType stageType) override;

		std::optional<ShaderFeatures> FindShaderFeatures(AssetHandle shaderHandle) override;

		bool HasCache(AssetHandle shaderHandle,
			ShaderTargetEnvironment targetEnvironment,
			ShaderStageType stage) override;

		std::optional<const ShaderEntry*> GetShaderEntry(AssetHandle shaderHandle) const;
		void SetShaderEntry(AssetHandle shaderHandle,
			ShaderFeatures features,
			ShaderProperties&& properties);

		std::filesystem::path GetCacheDirectoryPath(AssetHandle shaderHandle);
		std::string GetCacheFileName(std::string_view shaderName, ShaderTargetEnvironment targetEnvironemt, ShaderStageType stageType);
	private:
		std::unordered_map<AssetHandle, ShaderEntry> m_Entries;
	};
}