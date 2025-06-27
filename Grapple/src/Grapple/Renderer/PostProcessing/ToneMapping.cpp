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

		RenderGraphTextureId intermediateTexture = renderGraph.CreateTexture(TextureFormat::R11G11B10, "ToneMapping.IntermediateColorTexture");

		RenderGraphPassSpecifications toneMappingPass{};
		toneMappingPass.SetDebugName("ToneMapping");
		toneMappingPass.AddInput(viewport.ColorTextureId);
		toneMappingPass.AddOutput(intermediateTexture, 0);

		RenderGraphPassSpecifications blitPass{};
		blitPass.SetDebugName("ToneMappingBlit");
		BlitPass::ConfigureSpecifications(blitPass, intermediateTexture, viewport.ColorTextureId);

		renderGraph.AddPass(toneMappingPass, CreateRef<ToneMappingPass>(viewport.ColorTextureId));
		renderGraph.AddPass(blitPass, CreateRef<BlitPass>(intermediateTexture, viewport.ColorTextureId, TextureFiltering::Closest));
	}

	const SerializableObjectDescriptor& ToneMapping::GetSerializationDescriptor() const
	{
		return Grapple_SERIALIZATION_DESCRIPTOR_OF(ToneMapping);
	}



	ToneMappingPass::ToneMappingPass(RenderGraphTextureId colorTexture)
		: m_ColorTexture(colorTexture)
	{
		Grapple_PROFILE_FUNCTION();
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
			m_Material->SetTextureProperty(*colorTextureIndex, context.GetRenderGraphResourceManager().GetTexture(m_ColorTexture));

		commandBuffer->BeginRenderTarget(context.GetRenderTarget());
		commandBuffer->SetViewportAndScisors(Math::Rect(glm::vec2(0.0f, 0.0f), (glm::vec2)context.GetViewport().GetSize()));

		commandBuffer->ApplyMaterial(m_Material);
		commandBuffer->DrawMeshIndexed(RendererPrimitives::GetFullscreenQuadMesh(), 0, 0, 1);

		commandBuffer->EndRenderTarget();
	}
}
