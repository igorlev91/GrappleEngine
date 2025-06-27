#include "SSAO.h"

#include "Grapple/Scene/Scene.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/RendererPrimitives.h"
#include "Grapple/Renderer/CommandBuffer.h"
#include "Grapple/Renderer/ShaderLibrary.h"
#include "Grapple/Renderer/GraphicsContext.h"
#include "Grapple/Renderer/Material.h"

#include "Grapple/Renderer/Passes/BlitPass.h"

#include "Grapple/Platform/Vulkan/VulkanCommandBuffer.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "GrappleCore/Profiler/Profiler.h"

#include <random>

namespace Grapple
{
	Grapple_IMPL_TYPE(SSAO);

	SSAO::SSAO()
		: Bias(0.1f), Radius(0.5f), BlurSize(2.0f)
	{
	}

	void SSAO::RegisterRenderPasses(RenderGraph& renderGraph, const Viewport& viewport)
	{
		Grapple_PROFILE_FUNCTION();

		if (!IsEnabled())
			return;

		RenderGraphTextureId aoTexture = renderGraph.CreateTexture(TextureFormat::RF32, "SSAO.AOTexture");
		RenderGraphTextureId intermediateColorTexture = renderGraph.CreateTexture(TextureFormat::R11G11B10, "SSAO.IntermediateColorTexture");

		RenderGraphPassSpecifications ssaoMainPass{};
		ssaoMainPass.SetDebugName("SSAOMainPass");
		ssaoMainPass.AddInput(viewport.NormalsTextureId);
		ssaoMainPass.AddInput(viewport.DepthTextureId);
		ssaoMainPass.AddOutput(aoTexture, 0);

		RenderGraphPassSpecifications ssaoComposingPass{};
		ssaoComposingPass.SetDebugName("SSAOComposingPass");
		ssaoComposingPass.AddInput(aoTexture);
		ssaoComposingPass.AddInput(viewport.ColorTextureId);
		ssaoComposingPass.AddOutput(intermediateColorTexture, 0);

		RenderGraphPassSpecifications ssaoBlitPass{};
		ssaoBlitPass.SetDebugName("SSAOBlitPass");
		BlitPass::ConfigureSpecifications(ssaoBlitPass, intermediateColorTexture, viewport.ColorTextureId);

		renderGraph.AddPass(ssaoMainPass, CreateRef<SSAOMainPass>(viewport.NormalsTextureId, viewport.DepthTextureId));
		renderGraph.AddPass(ssaoComposingPass, CreateRef<SSAOComposingPass>(viewport.ColorTextureId, aoTexture));
		renderGraph.AddPass(ssaoBlitPass, CreateRef<BlitPass>(intermediateColorTexture, viewport.ColorTextureId, TextureFiltering::Closest));
	}

	const SerializableObjectDescriptor& SSAO::GetSerializationDescriptor() const
	{
		return Grapple_SERIALIZATION_DESCRIPTOR_OF(SSAO);
	}



	SSAOMainPass::SSAOMainPass(RenderGraphTextureId normalsTexture, RenderGraphTextureId depthTexture)
		: m_NormalsTexture(normalsTexture), m_DepthTexture(depthTexture)
	{
		Grapple_PROFILE_FUNCTION();
		std::optional<AssetHandle> shaderHandle = ShaderLibrary::FindShader("SSAO");
		if (shaderHandle && AssetManager::IsAssetHandleValid(shaderHandle.value()))
		{
			m_Material = Material::Create(AssetManager::GetAsset<Shader>(shaderHandle.value()));
		}
		else
		{
			Grapple_CORE_ERROR("SSAO: Failed to find SSAO shader");
		}

		auto result = Scene::GetActive()->GetPostProcessingManager().GetEffect<SSAO>();
		Grapple_CORE_ASSERT(result.has_value());
		m_Parameters = *result;
	}

	void SSAOMainPass::OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer)
	{
		Grapple_PROFILE_FUNCTION();

		commandBuffer->SetGlobalDescriptorSet(context.GetViewport().GlobalResources.CameraDescriptorSet, 0);
		commandBuffer->BeginRenderTarget(context.GetRenderTarget());

		auto biasIndex = m_Material->GetShader()->GetPropertyIndex("u_Params.Bias");
		auto radiusIndex = m_Material->GetShader()->GetPropertyIndex("u_Params.SampleRadius");

		auto normalsTextureIndex = m_Material->GetShader()->GetPropertyIndex("u_NormalsTexture");
		auto depthTextureIndex = m_Material->GetShader()->GetPropertyIndex("u_DepthTexture");

		m_Material->WritePropertyValue(*biasIndex, m_Parameters->Bias);
		m_Material->WritePropertyValue(*radiusIndex, m_Parameters->Radius);
		m_Material->SetTextureProperty(*normalsTextureIndex, context.GetRenderGraphResourceManager().GetTexture(m_NormalsTexture));
		m_Material->SetTextureProperty(*depthTextureIndex, context.GetRenderGraphResourceManager().GetTexture(m_DepthTexture));

		commandBuffer->SetViewportAndScisors(Math::Rect(glm::vec2(0.0f, 0.0f), (glm::vec2)context.GetViewport().GetSize()));

		commandBuffer->ApplyMaterial(m_Material);
		commandBuffer->DrawMeshIndexed(RendererPrimitives::GetFullscreenQuadMesh(), 0, 0, 1);

		commandBuffer->EndRenderTarget();
	}



	SSAOComposingPass::SSAOComposingPass(RenderGraphTextureId colorTexture, RenderGraphTextureId aoTexture)
		: m_ColorTexture(colorTexture), m_AOTexture(aoTexture)
	{
		Grapple_PROFILE_FUNCTION();
		std::optional<AssetHandle> shaderHandle = ShaderLibrary::FindShader("SSAOBlur");
		if (shaderHandle && AssetManager::IsAssetHandleValid(shaderHandle.value()))
		{
			m_Material = Material::Create(AssetManager::GetAsset<Shader>(shaderHandle.value()));
		}
		else
		{
			Grapple_CORE_ERROR("SSAO: Failed to find SSAO Blur shader");
		}

		auto result = Scene::GetActive()->GetPostProcessingManager().GetEffect<SSAO>();
		Grapple_CORE_ASSERT(result.has_value());
		m_Parameters = *result;
	}

	void SSAOComposingPass::OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer)
	{
		Grapple_PROFILE_FUNCTION();
		commandBuffer->BeginRenderTarget(context.GetRenderTarget());

		commandBuffer->SetViewportAndScisors(Math::Rect(glm::vec2(0.0f, 0.0f), (glm::vec2)context.GetViewport().GetSize()));
		glm::vec2 texelSize = glm::vec2(1.0f) / (glm::vec2)context.GetViewport().GetSize();

		auto colorTextureIndex = m_Material->GetShader()->GetPropertyIndex("u_ColorTexture");
		auto aoTextureIndex = m_Material->GetShader()->GetPropertyIndex("u_AOTexture");
		auto blurSizePropertyIndex = m_Material->GetShader()->GetPropertyIndex("u_Params.BlurSize");
		auto texelSizePropertyIndex = m_Material->GetShader()->GetPropertyIndex("u_Params.TexelSize");

		m_Material->WritePropertyValue(*blurSizePropertyIndex, m_Parameters->BlurSize);
		m_Material->WritePropertyValue(*texelSizePropertyIndex, texelSize);
		m_Material->SetTextureProperty(*aoTextureIndex, context.GetRenderGraphResourceManager().GetTexture(m_AOTexture));
		m_Material->SetTextureProperty(*colorTextureIndex, context.GetRenderGraphResourceManager().GetTexture(m_ColorTexture));

		commandBuffer->ApplyMaterial(m_Material);
		commandBuffer->DrawMeshIndexed(RendererPrimitives::GetFullscreenQuadMesh(), 0, 0, 1);

		commandBuffer->EndRenderTarget();
	}
}
