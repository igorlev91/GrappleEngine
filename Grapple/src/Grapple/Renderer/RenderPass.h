#pragma once

#include "Grapple/Renderer/RenderingContext.h"

namespace Grapple
{
	enum class RenderPassQueue
	{
		BeforeShadows,
		BeforeOpaqueGeometry,
		AfterOpaqueGeometry,

		PostProcessing,
	};

	class Grapple_API RenderPass
	{
	public:
		RenderPass(RenderPassQueue queue)
			: m_Queue(queue) {}

		virtual ~RenderPass() = default;
		virtual void OnRender(RenderingContext& context) = 0;

		inline RenderPassQueue GetQueue() const { return m_Queue; }
	private:
		RenderPassQueue m_Queue;
	};
}