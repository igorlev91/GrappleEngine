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
		Grapple_CORE_ASSERT(false);
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
