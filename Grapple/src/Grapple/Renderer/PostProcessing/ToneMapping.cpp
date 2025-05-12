#include "Tonemapping.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/RenderCommand.h"
#include "Grapple/Renderer/ShaderLibrary.h"

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

		m_Material = CreateRef<Material>(AssetManager::GetAsset<Shader>(shaderHandle.value()));
	}

	void ToneMapping::OnRender(RenderingContext& context)
	{
		if (!Enabled)
			return;

		Ref<FrameBuffer> output = context.RTPool.Get();

		context.RenderTarget->BindAttachmentTexture(0, 0);
		output->Bind();

		Renderer::DrawFullscreenQuad(m_Material);

		context.RenderTarget->Blit(output, 0, 0);
		context.RTPool.Release(output);
		context.RenderTarget->Bind();
	}
}
