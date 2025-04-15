#pragma once

#include "Grapple/Core/Core.h"
#include "Grapple/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Grapple
{
	enum class FrameBufferTextureFormat
	{
		RGB8,
		RGBA8,

		RedInteger,
	};

	struct FrameBufferAttachmentSpecifications
	{
		const FrameBufferTextureFormat Format;
		const TextureWrap Wrap;
		const TextureFiltering Filtering;
	};

	struct FrameBufferSpecifications
	{
		FrameBufferSpecifications(const std::initializer_list<FrameBufferAttachmentSpecifications>& attachments)
			: Width(0), Height(0), Attachments(attachments) {}
		FrameBufferSpecifications(uint32_t width, uint32_t height, const std::initializer_list<FrameBufferAttachmentSpecifications>& attachments)
			: Width(width), Height(height), Attachments(attachments) {}

		uint32_t Width;
		uint32_t Height;
		std::vector<FrameBufferAttachmentSpecifications> Attachments;
	};

	class Grapple_API FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void* GetColorAttachmentRendererId(uint32_t attachmentIndex) = 0;

		virtual uint32_t GetAttachmentsCount() = 0;
		virtual void ClearAttachment(uint32_t index, uint32_t value) = 0;
		virtual void ReadPixel(uint32_t attachmentIndex, uint32_t x, uint32_t y, void* pixelOutput) = 0;
		virtual void Blit(const Ref<FrameBuffer>& source, uint32_t destinationAttachment, uint32_t sourceAttachment) = 0;
		virtual void BindAttachmentTexture(uint32_t attachment, uint32_t slot = 0) = 0;

		virtual const FrameBufferSpecifications& GetSpecifications() const = 0;
	public:
		static Ref<FrameBuffer> Create(const FrameBufferSpecifications& specifications);
	};
}