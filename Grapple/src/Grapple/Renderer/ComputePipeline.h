#pragma once

#include "GrappleCore/Core.h"

#include "Grapple/Renderer/ComputeShader.h"

namespace Grapple
{
	struct ComputePipelineSpecifications
	{
		Ref<ComputeShader> Shader;
	};

	class ComputePipeline
	{
	public:
		virtual const ComputePipelineSpecifications& GetSpecifications() const = 0;
	public:
		static Ref<ComputePipeline> Create(const ComputePipelineSpecifications& specifications);
	};
}
