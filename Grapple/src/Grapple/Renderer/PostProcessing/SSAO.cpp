#include "SSAO.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/ShaderLibrary.h"

#include "Grapple/AssetManager/AssetManager.h"

namespace Grapple
{
	Grapple_SERIALIZABLE_IMPL(SSAO);

	SSAO::SSAO()
		: m_BiasPropertyIndex({}), m_RadiusPropertyIndex({}), Bias(0.00001f), Radius(0.05f)
	{
		std::optional<AssetHandle> shaderHandle = ShaderLibrary::FindShader("SSAO");
		if (!shaderHandle || !AssetManager::IsAssetHandleValid(shaderHandle.value()))
		{
			Grapple_CORE_ERROR("SSAO: Failed to find SSAO shader");
			return;
		}

		m_Material = CreateRef<Material>(AssetManager::GetAsset<Shader>(shaderHandle.value()));
		m_BiasPropertyIndex = m_Material->GetShader()->GetPropertyIndex("u_Params.Bias");
		m_RadiusPropertyIndex = m_Material->GetShader()->GetPropertyIndex("u_Params.SampleRadius");
	}

	void SSAO::OnRender(RenderingContext& context)
	{
		Ref<FrameBuffer> aoTarget = nullptr;
		size_t targetIndex = 0;
		if (&Renderer::GetCurrentViewport() == &Renderer::GetMainViewport())
			targetIndex = 0;
		else
			targetIndex = 1;

		aoTarget = m_AOTargets[targetIndex];

		const Viewport& currentViewport = Renderer::GetCurrentViewport();
		if (!aoTarget)
		{
			FrameBufferSpecifications specifications = FrameBufferSpecifications(
				currentViewport.GetSize().x, currentViewport.GetSize().y,
				{ {FrameBufferTextureFormat::RF32, TextureWrap::Clamp, TextureFiltering::Linear } }
			);

			aoTarget = FrameBuffer::Create(specifications);
			m_AOTargets[targetIndex] = aoTarget;
		}
		else
		{
			const auto& specifications = aoTarget->GetSpecifications();
			if (specifications.Width != currentViewport.GetSize().x || specifications.Height != currentViewport.GetSize().y)
			{
				aoTarget->Resize(currentViewport.GetSize().x, currentViewport.GetSize().y);
			}
		}

		aoTarget->Bind();

		currentViewport.RenderTarget->BindAttachmentTexture(1, 0);

		if (&currentViewport == &Renderer::GetMainViewport())
			currentViewport.RenderTarget->BindAttachmentTexture(2, 1);
		else
			currentViewport.RenderTarget->BindAttachmentTexture(3, 1);

		if (m_BiasPropertyIndex)
			m_Material->WritePropertyValue(*m_BiasPropertyIndex, Bias);
		
		if (m_RadiusPropertyIndex)
			m_Material->WritePropertyValue(*m_RadiusPropertyIndex, Radius);
		
		Renderer::DrawFullscreenQuad(m_Material);

		context.RenderTarget->Blit(aoTarget, 0, 0);
		context.RenderTarget->Bind();
	}
}
