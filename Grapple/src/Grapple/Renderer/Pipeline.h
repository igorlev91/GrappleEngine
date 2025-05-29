#pragma once

#include "Grapple/Renderer/Buffer.h"
#include "Grapple/Renderer/Shader.h"

namespace Grapple
{
	struct PipelineSpecifications
	{
		BufferLayout InputLayout;
		Ref<Shader> Shader;

		CullingMode Culling = CullingMode::Back;
		bool DepthTest = true;
		bool DepthWrite = true;
		DepthComparisonFunction DepthFunction = DepthComparisonFunction::Less;
	};

	class Pipeline
	{
	public:
		virtual const PipelineSpecifications& GetSpecifications() const = 0;
	public:
		static Ref<Pipeline> Create(const PipelineSpecifications& specifications);
	};
}
