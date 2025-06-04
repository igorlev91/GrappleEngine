#pragma once

#include "GrappleCore/Serialization/Serialization.h"
#include "GrappleCore/Serialization/SerializationStream.h"

#include "Grapple/Renderer/RenderPass.h"
#include "Grapple/Renderer/Shader.h"
#include "Grapple/Renderer/Material.h"

#include "Grapple/Renderer/RenderGraph/RenderGraphPass.h"

namespace Grapple
{
	class Grapple_API Vignette : public RenderPass
	{
	public:
		Grapple_TYPE;

		Vignette();

		void OnRender(RenderingContext& context) override;
	public:
		bool Enabled;
		glm::vec4 Color;
		float Radius;
		float Smoothness;
	private:
		Ref<Material> m_Material;
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
		Ref<Material> m_Material;
		glm::vec4 m_Color = glm::vec4(1.0f);
		float m_Radius = 1.0f;
		float m_Smoothness = 1.0f;
	};
}