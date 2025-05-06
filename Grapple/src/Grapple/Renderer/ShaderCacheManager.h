#pragma once

#include "GrappleCore/Core.h"

#include "Grapple/AssetManager/Asset.h"
#include "Grapple/Renderer/RendererAPI.h"

#include "Grapple/Renderer/Shader.h"

#include <vector>
#include <optional>

namespace Grapple
{
	class Grapple_API ShaderCacheManager
	{
	public:
		virtual void SetCache(AssetHandle shaderHandle,
			ShaderTargetEnvironment targetEnvironment,
			ShaderStageType stageType,
			const std::vector<uint32_t>& compiledShader) = 0;

		virtual std::optional<std::vector<uint32_t>> FindCache(AssetHandle shaderHandle,
			ShaderTargetEnvironment targetEnvironment,
			ShaderStageType stageType) = 0;

		virtual std::optional<ShaderFeatures> FindShaderFeatures(AssetHandle shaderHandle) = 0;

		virtual bool HasCache(AssetHandle shaderHandle,
			ShaderTargetEnvironment targetEnvironment,
			ShaderStageType stage) = 0;
	public:
		static void Uninitialize();
		static void SetInstance(Scope<ShaderCacheManager>&& cacheManager);
		static const Scope<ShaderCacheManager>& GetInstance();
	};
}