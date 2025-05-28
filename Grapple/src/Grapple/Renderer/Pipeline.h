#pragma once

#include "Grapple/Renderer/Buffer.h"
#include "Grapple/Renderer/Shader.h"

namespace Grapple
{
	struct PipelineSpecifications
	{
		BufferLayout InputLayout;
		Ref<Shader> Shader;
	};

	class Pipeline
	{
	public:
		virtual const PipelineSpecifications& GetSpecifications() const = 0;
	public:
		static Ref<Pipeline> Create(const PipelineSpecifications& specifications);
	};
}
