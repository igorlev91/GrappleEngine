#pragma once

#include "GrappleCore/Core.h"

namespace Grapple
{
	class RenderGraph;
	class Viewport;
	class SerializableObjectDescriptor;
	class PostProcessingManager;

	class Grapple_API PostProcessingEffect
	{
	public:
		virtual ~PostProcessingEffect() = default;

		void OnAttach(PostProcessingManager& postProcessingManager);

		virtual void RegisterRenderPasses(RenderGraph& renderGraph, const Viewport& viewport) = 0;
		virtual const SerializableObjectDescriptor& GetSerializationDescriptor() const = 0;
	public:
		inline bool IsEnabled() const { return m_IsEnabled; }

		void SetEnabled(bool enabled);
	private:
		PostProcessingManager* m_PostProcessingManager = nullptr;
		bool m_IsEnabled = false;
	};
}
