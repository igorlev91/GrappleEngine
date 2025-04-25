#include "Vignette.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/RenderCommand.h"

namespace Grapple
{
	Grapple_IMPL_TYPE(Vignette,
		Grapple_FIELD(Vignette, Enabled),
		Grapple_FIELD(Vignette, Color),
		Grapple_FIELD(Vignette, Radius),
		Grapple_FIELD(Vignette, Smoothness),
	);

	Vignette::Vignette()
		: Enabled(false), Color(0.0f, 0.0f, 0.0f, 0.5f), Radius(1.0f), Smoothness(1.0f)
	{
		m_Shader = Shader::Create("assets/Shaders/Vignette.glsl");
	}

	void Vignette::OnRender(RenderingContext& context)
	{
		if (!Enabled)
			return;

		m_Shader->Bind();
		m_Shader->SetFloat4("u_Params.Color", Color);
		m_Shader->SetFloat("u_Params.Radius", Radius);
		m_Shader->SetFloat("u_Params.Smoothness", Smoothness);

		RenderCommand::DrawIndexed(Renderer::GetFullscreenQuad());
	}
}
