#include "Tonemapping.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/ShaderLibrary.h"
#include "Grapple/Renderer/CommandBuffer.h"
#include "Grapple/Renderer/GraphicsContext.h"
#include "Grapple/Renderer/RendererPrimitives.h"
#include "Grapple/Renderer/Passes/BlitPass.h"

#include "GrappleCore/Profiler/Profiler.h"

namespace Grapple
{
	Grapple_IMPL_TYPE(ToneMapping);

	ToneMapping::ToneMapping()
	{
	}

	void ToneMapping::RegisterRenderPasses(RenderGraph& renderGraph, const Viewport& viewport)
	{
		Grapple_PROFILE_FUNCTION();

		if (!IsEnabled())
			return;

		TextureSpecifications colorTextureSpecifications{};
		colorTextureSpecifications.Filtering = TextureFiltering::Closest;
		colorTextureSpecifications.Format = TextureFormat::R11G11B10;
		colorTextureSpecifications.Wrap = TextureWrap::Clamp;
		colorTextureSpecifications.GenerateMipMaps = false;
		colorTextureSpecifications.Width = viewport.GetSize().x;
		colorTextureSpecifications.Height = viewport.GetSize().y;
		colorTextureSpecifications.Usage = TextureUsage::RenderTarget | TextureUsage::Sampling;

		Ref<Texture> intermediateTexture = Texture::Create(colorTextureSpecifications);
		intermediateTexture->SetDebugName("ToneMapping.IntermediateColorTexture");

		RenderGraphPassSpecifications toneMappingPass{};
		toneMappingPass.SetDebugName("ToneMapping");
		toneMappingPass.AddInput(viewport.ColorTexture);
		toneMappingPass.AddOutput(intermediateTexture, 0);

		RenderGraphPassSpecifications blitPass{};
		blitPass.SetDebugName("ToneMappingBlit");
		BlitPass::ConfigureSpecifications(blitPass, intermediateTexture, viewport.ColorTexture);

		renderGraph.AddPass(toneMappingPass, CreateRef<ToneMappingPass>(viewport.ColorTexture));
		renderGraph.AddPass(blitPass, CreateRef<BlitPass>(intermediateTexture, viewport.ColorTexture, TextureFiltering::Closest));
	}

	const SerializableObjectDescriptor& ToneMapping::GetSerializationDescriptor() const
	{
		return Grapple_SERIALIZATION_DESCRIPTOR_OF(ToneMapping);
	}



	ToneMappingPass::ToneMappingPass(Ref<Texture> colorTexture)
		: m_ColorTexture(colorTexture)
	{
		std::optional<AssetHandle> shaderHandle = ShaderLibrary::FindShader("AcesToneMapping");
		if (!shaderHandle || !AssetManager::IsAssetHandleValid(shaderHandle.value()))
		{
			Grapple_CORE_ERROR("ToneMapping: Failed to find ToneMapping shader");
			return;
		}

		Ref<Shader> shader = AssetManager::GetAsset<Shader>(shaderHandle.value());
		m_Material = Material::Create(shader);
	}

	void ToneMappingPass::OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer)
	{
		Grapple_PROFILE_FUNCTION();

		if (m_Material == nullptr)
			return;

		auto colorTextureIndex = m_Material->GetShader()->GetPropertyIndex("u_ScreenBuffer");
		if (colorTextureIndex)
			m_Material->GetPropertyValue<TexturePropertyValue>(*colorTextureIndex).SetTexture(m_ColorTexture);

		commandBuffer->BeginRenderTarget(context.GetRenderTarget());
		commandBuffer->SetViewportAndScisors(Math::Rect(glm::vec2(0.0f, 0.0f), (glm::vec2)context.GetViewport().GetSize()));

		commandBuffer->ApplyMaterial(m_Material);
		commandBuffer->DrawIndexed(RendererPrimitives::GetFullscreenQuadMesh(), 0, 0, 1);

		commandBuffer->EndRenderTarget();
	}
}
