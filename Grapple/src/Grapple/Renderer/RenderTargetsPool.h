#pragma once

#include "GrappleCore/Core.h"
#include "GrappleCore/Assert.h"

#include "Grapple/Renderer/FrameBuffer.h"

#include <vector>

namespace Grapple
{
	class Grapple_API RenderTargetsPool
	{
	public:
		RenderTargetsPool() = default;

		Ref<FrameBuffer> Get();
		void Release(const Ref<FrameBuffer>& renderTarget);
		void SetRenderTargetsSize(glm::ivec2 size);

		void SetSpecifications(const FrameBufferSpecifications& specs);
	private:
		FrameBufferSpecifications m_Specifications;
		std::vector<Ref<FrameBuffer>> m_Pool;
	};
}