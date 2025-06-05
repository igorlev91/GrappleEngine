#pragma once

#include "Grapple/Renderer/RenderGraph/RenderGraph.h"
#include "Grapple/Renderer/PostProcessing/PostProcessingEffect.h"

#include <optional>

namespace Grapple
{
	class Grapple_API PostProcessingManager
	{
	public:
		struct PostProcessingEntry
		{
			const SerializableObjectDescriptor* Descriptor = nullptr;
			Ref<PostProcessingEffect> Effect = nullptr;
		};

		void AddEffect(Ref<PostProcessingEffect> effect);
		void RegisterRenderPasses(RenderGraph& renderGraph, const Viewport& viewport);

		std::optional<Ref<PostProcessingEffect>> FindEffect(const SerializableObjectDescriptor& descriptor) const;

		template<typename T>
		std::optional<Ref<T>> GetEffect()
		{
			std::optional<Ref<PostProcessingEffect>> effect = FindEffect(Grapple_SERIALIZATION_DESCRIPTOR_OF(T));
			if (effect)
				return As<T>(*effect);

			return {};
		}

		inline const std::vector<PostProcessingEntry>& GetEntries() const { return m_Entries; }
	private:
		std::vector<PostProcessingEntry> m_Entries;
	};
}