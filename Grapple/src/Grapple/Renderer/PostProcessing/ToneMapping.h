#pragma once

#include "GrappleCore/Serialization/TypeSerializer.h"
#include "GrappleCore//Serialization/SerializationStream.h"

#include "Grapple/Renderer/RenderPass.h"
#include "Grapple/Renderer/Material.h"

#include "Grapple/Renderer/RenderGraph/RenderGraphPass.h"

namespace Grapple
{
	class Grapple_API ToneMapping : public RenderPass
	{
	public:
		Grapple_TYPE;

		ToneMapping();

		void OnRender(RenderingContext& context) override;
	public:
		bool Enabled;
	private:
		std::optional<uint32_t> m_ColorTexture;
		Ref<Material> m_Material;
	};

	template<>
	struct TypeSerializer<ToneMapping>
	{
		void OnSerialize(ToneMapping& toneMapping, SerializationStream& stream)
		{
			stream.Serialize("Enabled", SerializationValue(toneMapping.Enabled));
		}
	};



	class Grapple_API ToneMappingPass : public RenderGraphPass
	{
	public:
		ToneMappingPass(Ref<Texture> colorTexture);

		void OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer) override;
	private:
		Ref<Material> m_Material = nullptr;
		Ref<Texture> m_ColorTexture = nullptr;
	};
}