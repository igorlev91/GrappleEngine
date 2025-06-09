#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/Renderer/FrameBuffer.h"
#include "Grapple/Renderer/RenderData.h"

#include "Grapple/Renderer/RenderGraph/RenderGraph.h"

#include <vector>
#include <glm/glm.hpp>

namespace Grapple
{
	class Texture;
	class Grapple_API Viewport
	{
	public:
		inline glm::ivec2 GetPosition() const { return m_Position; }
		inline glm::ivec2 GetSize() const { return m_Size; }

		inline float GetAspectRatio() const { return (float)m_Size.x / (float)m_Size.y; }

		void Resize(glm::ivec2 position, glm::ivec2 size);
	public:
		bool PostProcessingEnabled = true;
		bool ShadowMappingEnabled = true;
		bool DebugRendering = false;

		RenderData FrameData;
		Ref<FrameBuffer> RenderTarget = nullptr;

		RenderGraph Graph;

		Ref<Texture> ColorTexture = nullptr;
		Ref<Texture> NormalsTexture = nullptr;
		Ref<Texture> DepthTexture = nullptr;
	private:
		glm::ivec2 m_Position = glm::ivec2(0, 0);
		glm::ivec2 m_Size = glm::ivec2(0, 0);
	};
}