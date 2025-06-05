#include "PostProcessingEffect.h"

#include "Grapple/Renderer/PostProcessing/PostProcessingManager.h"

namespace Grapple
{
	void PostProcessingEffect::OnAttach(PostProcessingManager& postProcessingManager)
	{
		m_PostProcessingManager = &postProcessingManager;
	}

	void PostProcessingEffect::SetEnabled(bool enabled)
	{
		if (m_IsEnabled == enabled)
			return;

		m_IsEnabled = enabled;

		m_PostProcessingManager->MarkAsDirty();
	}
}