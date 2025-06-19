#pragma once

#include "Grapple/Renderer/Buffer.h"
#include "Grapple/Renderer/Shader.h"

namespace Grapple
{
	struct PipelineInputLayoutElement
	{
	public:
		PipelineInputLayoutElement(uint32_t binding, uint32_t location, ShaderDataType type)
			: Binding(binding), Location(location), Type(type) {}

	public:
		uint32_t Binding = 0;
		uint32_t Location = 0;
		ShaderDataType Type = ShaderDataType::Float;

		size_t Offset = 0;
	};

	struct PipelineInputLayout
	{
	public:
		PipelineInputLayout() {}
		PipelineInputLayout(const std::initializer_list<PipelineInputLayoutElement>& elements)
			: Elements(elements)
		{
			for (auto& element : Elements)
			{
				element.Offset = Stride;
				Stride += ShaderDataTypeSize(element.Type);
			}
		}
	public:
		size_t Stride = 0;
		std::vector<PipelineInputLayoutElement> Elements;
	};

	enum class PrimitiveTopology
	{
		Triangles,
		Lines,
	};

	struct PipelineSpecifications
	{
		PipelineInputLayout InputLayout;
		Ref<Shader> Shader;

		CullingMode Culling = CullingMode::Back;
		BlendMode Blending = BlendMode::Opaque;
		PrimitiveTopology Topology = PrimitiveTopology::Triangles;
		bool DepthTest = true;
		bool DepthWrite = true;
		bool DepthBiasEnabled = false;
		bool DepthClampEnabled = false;
		float DepthBiasSlopeFactor = 0.0f;
		float DepthBiasConstantFactor = 0.0f;
		DepthComparisonFunction DepthFunction = DepthComparisonFunction::Less;
	};

	class Grapple_API Pipeline
	{
	public:
		virtual const PipelineSpecifications& GetSpecifications() const = 0;
	public:
		static Ref<Pipeline> Create(const PipelineSpecifications& specifications);
	};
}
