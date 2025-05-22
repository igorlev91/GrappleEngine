#include "AtmospherePass.h"

#include "Grapple/Renderer/Renderer.h"

namespace Grapple
{
	Grapple_IMPL_TYPE(AtmospherePass);

	void AtmospherePass::OnRender(RenderingContext& context)
	{
		if (!AtmosphereMaterial || !Enabled || !Renderer::GetCurrentViewport().PostProcessingEnabled)
			return;

		Renderer::DrawFullscreenQuad(AtmosphereMaterial);
	}
}
