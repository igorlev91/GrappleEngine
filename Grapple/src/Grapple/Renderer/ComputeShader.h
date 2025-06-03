#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/AssetManager/Asset.h"
#include "Grapple/Renderer/ShaderMetadata.h"

namespace Grapple
{
	struct ComputeShaderMetadata
	{
		std::string Name;
		glm::uvec3 LocalGroupSize = glm::uvec3(1u);
		ShaderPushConstantsRange PushConstantsRange;
	};

	class Grapple_API ComputeShader : public Asset
	{
	public:
		Grapple_SERIALIZABLE;
		Grapple_ASSET;

		ComputeShader();
		virtual ~ComputeShader();

		virtual Ref<const ComputeShaderMetadata> GetMetadata() const = 0;
		virtual void Load() = 0;
		virtual bool IsLoaded() const = 0;
	public:
		static Ref<ComputeShader> Create();
	};
}
