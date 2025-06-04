#pragma once

#include "GrappleCore/Core.h"
#include "GrappleCore/Assert.h"

#include "GrappleCore/Collections/Span.h"
#include "Grapple/Renderer/FrameBuffer.h"

#include <vector>
#include <unordered_map>

template<>
struct std::hash<Grapple::Span<Grapple::TextureFormat>>
{
	size_t operator()(const Grapple::Span<Grapple::TextureFormat>& formats) const
	{
		Grapple_CORE_ASSERT(!formats.IsEmpty());

		using UnderlyinfFormatType = std::underlying_type_t<Grapple::TextureFormat>;
		auto hash = std::hash<UnderlyinfFormatType>();

		size_t result = hash((UnderlyinfFormatType)formats[0]);
		for (size_t i = 1; i < formats.GetSize(); i++)
		{
			Grapple::CombineHashes(result, (UnderlyinfFormatType)formats[i]);
		}

		return result;
	}
};

namespace Grapple
{
	class Grapple_API RenderTargetsPool
	{
	public:
		RenderTargetsPool() = default;

		Ref<FrameBuffer> Get();
		Ref<FrameBuffer> GetFullscreen(const Span<TextureFormat>& formats);

		void ReturnFullscreen(Span<TextureFormat> formats, Ref<FrameBuffer> renderTarget);

		void Release(const Ref<FrameBuffer>& renderTarget);
		void SetRenderTargetsSize(glm::ivec2 size);

		void SetSpecifications(const FrameBufferSpecifications& specs);
		void OnViewportResize(glm::uvec2 newSize);

		void Clear();
	private:
		struct FullscreenRenderTargetEntry
		{
			std::vector<TextureFormat> Formats;
			FrameBufferSpecifications Specifications;
			std::vector<Ref<FrameBuffer>> Pool;
		};

		glm::uvec2 m_ViewportSize = { 0, 0 };

		FrameBufferSpecifications m_Specifications;
		std::vector<Ref<FrameBuffer>> m_Pool;

		std::unordered_map<Span<TextureFormat>, FullscreenRenderTargetEntry> m_FullscreenTargets;
	};
}