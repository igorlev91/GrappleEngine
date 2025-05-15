#include "Tonemapping.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/RenderCommand.h"
#include "Grapple/Renderer/ShaderLibrary.h"

#include "GrappleCore/Profiler/Profiler.h"

namespace Grapple
{
	Grapple_IMPL_TYPE(ToneMapping);

	ToneMapping::ToneMapping()
		: Enabled(false)
	{
		std::optional<AssetHandle> shaderHandle = ShaderLibrary::FindShader("AcesToneMapping");
		if (!shaderHandle || !AssetManager::IsAssetHandleValid(shaderHandle.value()))
		{
			Grapple_CORE_ERROR("ToneMapping: Failed to find ToneMapping shader");
			return;
		}

		Ref<Shader> shader = AssetManager::GetAsset<Shader>(shaderHandle.value());
		m_Material = CreateRef<Material>(shader);

		m_ColorTexture = shader->GetPropertyIndex("u_ScreenBuffer");
	}

	void ToneMapping::OnRender(RenderingContext& context)
	{
		Grapple_PROFILE_FUNCTION();

		if (!Enabled)
			return;

		Ref<FrameBuffer> output = context.RTPool.Get();

		if (m_ColorTexture)
			m_Material->GetPropertyValue<TexturePropertyValue>(*m_ColorTexture).SetFrameBuffer(context.RenderTarget, 0);

		output->Bind();

		Renderer::DrawFullscreenQuad(m_Material);

		context.RenderTarget->Blit(output, 0, 0);
		context.RTPool.Release(output);
		context.RenderTarget->Bind();
	}
}
