#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/Renderer/FrameBuffer.h"
#include "Grapple/Renderer/RenderData.h"
#include "Grapple/Renderer/RenderTargetsPool.h"

#include <vector>
#include <glm/glm.hpp>

namespace Grapple
{
	class Grapple_API Viewport
	{
	public:
		Viewport();

		inline glm::ivec2 GetPosition() const { return m_Position; }
		inline glm::ivec2 GetSize() const { return m_Size; }

		inline float GetAspectRatio() const { return (float)m_Size.x / (float)m_Size.y; }

		void Resize(glm::ivec2 position, glm::ivec2 size);
	public:
		bool PostProcessingEnabled = true;
		bool ShadowMappingEnabled = true;

		RenderData FrameData;
		Ref<FrameBuffer> RenderTarget;
		RenderTargetsPool RTPool;

		uint32_t ColorAttachmentIndex = UINT32_MAX;
		uint32_t NormalsAttachmentIndex = UINT32_MAX;
		uint32_t DepthAttachmentIndex = UINT32_MAX;
	private:
		bool m_IsDirty;

		glm::ivec2 m_Position;
		glm::ivec2 m_Size;
	};
}