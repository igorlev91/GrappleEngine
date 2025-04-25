#include "Tonemapping.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/RenderCommand.h"

namespace Grapple
{
	ToneMapping::ToneMapping()
		: Enabled(false)
	{
		m_Shader = Shader::Create("assets/Shaders/AcesToneMapping.glsl");
	}

	void ToneMapping::OnRender(RenderingContext& context)
	{
		if (!Enabled)
			return;

		Ref<FrameBuffer> output = context.RTPool.Get();

		context.RenderTarget->BindAttachmentTexture(0, 0);
		output->Bind();

		m_Shader->Bind();
		RenderCommand::DrawIndexed(Renderer::GetFullscreenQuad());

		context.RenderTarget->Bind();
		context.RenderTarget->Blit(output, 0, 0);
		context.RTPool.Release(output);
	}
}
