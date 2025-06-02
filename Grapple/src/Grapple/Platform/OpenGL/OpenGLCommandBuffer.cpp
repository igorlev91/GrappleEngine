#include "OpenGLCommandBuffer.h"

#include "Grapple/Renderer/Material.h"
#include "Grapple/Renderer/RenderCommand.h"

#include "Grapple/Platform/OpenGL/OpenGLGPUTimer.h"

#include <glad/glad.h>

namespace Grapple
{
	OpenGLCommandBuffer::~OpenGLCommandBuffer()
	{
		Grapple_CORE_ASSERT(m_CurrentRenderTarget == nullptr);
	}

	void OpenGLCommandBuffer::BeginRenderTarget(const Ref<FrameBuffer> frameBuffer)
	{
		Grapple_CORE_ASSERT(m_CurrentRenderTarget == nullptr);
		m_CurrentRenderTarget = As<OpenGLFrameBuffer>(frameBuffer);
		m_CurrentRenderTarget->Bind();
	}

	void OpenGLCommandBuffer::EndRenderTarget()
	{
		Grapple_CORE_ASSERT(m_CurrentRenderTarget);

		// TODO: unbinding current render targets causes flickering while nothing is being rendered except window title bar
		//m_CurrentRenderTarget->Unbind();

		m_CurrentRenderTarget = nullptr;
	}

	void OpenGLCommandBuffer::ApplyMaterial(const Ref<const Material>& material)
	{
		Grapple_CORE_ASSERT(m_CurrentRenderTarget);

		Ref<Shader> shader = material->GetShader();
		Grapple_CORE_ASSERT(shader);

		RenderCommand::ApplyMaterial(material);

		FrameBufferAttachmentsMask shaderOutputsMask = 0;
		for (uint32_t output : shader->GetOutputs())
			shaderOutputsMask |= (1 << output);

		m_CurrentRenderTarget->SetWriteMask(shaderOutputsMask);
	}

	void OpenGLCommandBuffer::SetViewportAndScisors(Math::Rect viewportRect)
	{
		glViewport((int32_t)viewportRect.Min.x, (int32_t)viewportRect.Min.y, (int32_t)viewportRect.GetWidth(), (int32_t)viewportRect.GetHeight());
		glScissor((int32_t)viewportRect.Min.x, (int32_t)viewportRect.Min.y, (int32_t)viewportRect.GetWidth(), (int32_t)viewportRect.GetHeight());
	}

	void OpenGLCommandBuffer::DrawIndexed(const Ref<const Mesh>& mesh, uint32_t subMeshIndex, uint32_t baseInstance, uint32_t instanceCount)
	{
		RenderCommand::DrawInstancesIndexed(mesh, subMeshIndex, instanceCount, baseInstance);
	}

	void OpenGLCommandBuffer::Blit(Ref<FrameBuffer> source, uint32_t sourceAttachment, Ref<FrameBuffer> destination, uint32_t destinationAttachment, TextureFiltering filter)
	{
		Grapple_CORE_ASSERT(source && destination);
		Grapple_CORE_ASSERT(destinationAttachment < destination->GetAttachmentsCount());
		Grapple_CORE_ASSERT(sourceAttachment < source->GetAttachmentsCount());

		Ref<OpenGLFrameBuffer> sourceFrameBuffer = As<OpenGLFrameBuffer>(source);
		Ref<OpenGLFrameBuffer> destinationFrameBuffer = As<OpenGLFrameBuffer>(destination);

		const auto& sourceSpec = source->GetSpecifications();
		const auto& destinationSpec = destination->GetSpecifications();

		bool sourceAttachmentIsDepth = IsDepthFormat(sourceSpec.Attachments[sourceAttachment].Format);
		bool destinationAttachmentIsDepth = IsDepthFormat(destinationSpec.Attachments[destinationAttachment].Format);

		Grapple_CORE_ASSERT(sourceAttachmentIsDepth == destinationAttachmentIsDepth);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		glBindFramebuffer(GL_READ_FRAMEBUFFER, sourceFrameBuffer->GetId());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destinationFrameBuffer->GetId());

		glReadBuffer(GL_COLOR_ATTACHMENT0 + sourceAttachment);
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + destinationAttachment);

		GLenum blitFilter = GL_NEAREST;
		switch (filter)
		{
		case TextureFiltering::Closest:
			blitFilter = GL_NEAREST;
			break;
		case TextureFiltering::Linear:
			blitFilter = GL_LINEAR;
			break;
		default:
			Grapple_CORE_ASSERT(false);
		}

		glBlitFramebuffer(
			0, 0, 
			sourceSpec.Width, 
			sourceSpec.Height, 
			0, 0, 
			destinationSpec.Width,
			destinationSpec.Height,
			sourceAttachmentIsDepth
				? GL_DEPTH_BUFFER_BIT
				: GL_COLOR_BUFFER_BIT,
			blitFilter);

		destination->SetWriteMask(destination->GetWriteMask());

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLCommandBuffer::StartTimer(Ref<GPUTimer> timer)
	{
		As<OpenGLGPUTImer>(timer)->Start();
	}

	void OpenGLCommandBuffer::StopTimer(Ref<GPUTimer> timer)
	{
		As<OpenGLGPUTImer>(timer)->Stop();
	}
}
