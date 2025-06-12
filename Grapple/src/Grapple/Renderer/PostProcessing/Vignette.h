#pragma once

#include "GrappleCore/Serialization/Serialization.h"
#include "GrappleCore/Serialization/SerializationStream.h"

#include "Grapple/Renderer/PostProcessing/PostProcessingEffect.h"
#include "Grapple/Renderer/RenderGraph/RenderGraphPass.h"

namespace Grapple
{
	class Material;

	class Grapple_API Vignette : public PostProcessingEffect
	{
	public:
		Grapple_TYPE;

		Vignette();

		void RegisterRenderPasses(RenderGraph& renderGraph, const Viewport& viewport) override;
		const SerializableObjectDescriptor& GetSerializationDescriptor() const override;
	public:
		glm::vec4 Color;
		float Radius;
		float Smoothness;
	};

	template<>
	struct TypeSerializer<Vignette>
	{
		Grapple_API static void OnSerialize(Vignette& vignette, SerializationStream& stream);
	};

	class Grapple_API VignettePass : public RenderGraphPass
	{
	public:
		VignettePass();
		void OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer) override;
	private:
		Ref<Vignette> m_Parameters = nullptr;
		Ref<Material> m_Material = nullptr;
	};
}