#pragma once

#include "GrappleCore/Core.h"

#include "Grapple/AssetManager/Asset.h"
#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Renderer/ShaderMetadata.h"

#include <glm/glm.hpp>

#include <filesystem>
#include <optional>

namespace Grapple
{
	class Grapple_API Shader : public Asset
	{
	public:
		Grapple_SERIALIZABLE;
		Grapple_ASSET;

		Shader()
			: Asset(AssetType::Shader) {}

		virtual ~Shader() = default;

		virtual void Load() = 0;
		virtual bool IsLoaded() const = 0;

		virtual Ref<const ShaderMetadata> GetMetadata() const = 0;
		virtual const ShaderProperties& GetProperties() const = 0;
		virtual const ShaderOutputs& GetOutputs() const = 0;
		virtual ShaderFeatures GetFeatures() const = 0;
		virtual std::optional<uint32_t> GetPropertyIndex(std::string_view name) const = 0;
	public:
		static Ref<Shader> Create();
	};
}