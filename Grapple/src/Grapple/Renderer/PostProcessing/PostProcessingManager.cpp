#include "PostProcessingManager.h"

#include "GrappleCore/Assert.h"

namespace Grapple
{
	void PostProcessingManager::AddEffect(Ref<PostProcessingEffect> effect)
	{
		auto& entry = m_Entries.emplace_back();
		entry.Effect = effect;
		entry.Descriptor = &effect->GetSerializationDescriptor();

		entry.Effect->OnAttach(*this);

		if (m_Initialized)
			m_IsDirty = true;
	}

	void PostProcessingManager::RegisterRenderPasses(RenderGraph& renderGraph, const Viewport& viewport)
	{
		Grapple_CORE_ASSERT(!m_Initialized || m_IsDirty);

		for (const auto& entry : m_Entries)
		{
			entry.Effect->RegisterRenderPasses(renderGraph, viewport);
		}

		m_Initialized = true;
		m_IsDirty = false;
	}

	std::optional<Ref<PostProcessingEffect>> PostProcessingManager::FindEffect(const SerializableObjectDescriptor& descriptor) const
	{
		for (const auto& entry : m_Entries)
		{
			if (entry.Descriptor == &descriptor)
			{
				return entry.Effect;
			}
		}

		return {};
	}

	void PostProcessingManager::MarkAsDirty()
	{
		m_IsDirty = true;
	}
}
