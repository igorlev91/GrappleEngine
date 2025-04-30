#include "Tonemapping.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/RenderCommand.h"

namespace Grapple
{
	Grapple_IMPL_TYPE(ToneMapping, Grapple_FIELD(ToneMapping, Enabled));

	ToneMapping::ToneMapping()
		: Enabled(false)
	{
		m_Material = CreateRef<Material>(Shader::Create("assets/Shaders/AcesToneMapping.glsl"));
		m_Material->Features.DepthTesting = false;
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
