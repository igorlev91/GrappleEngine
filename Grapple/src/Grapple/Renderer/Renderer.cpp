#include "Renderer.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Renderer/UniformBuffer.h"
#include "Grapple/Renderer2D/Renderer2D.h"

namespace Grapple
{
	struct RendererData
	{
		Viewport* MainViewport;
		Viewport* CurrentViewport;

		Ref<UniformBuffer> CameraBuffer;
	};

	RendererData s_RendererData;

	void Renderer::Initialize()
	{
		s_RendererData.MainViewport = nullptr;
		s_RendererData.CurrentViewport = nullptr;

		s_RendererData.CameraBuffer = UniformBuffer::Create(sizeof(CameraData), 0);
	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::SetMainViewport(Viewport& viewport)
	{
		s_RendererData.MainViewport = &viewport;
	}

	void Renderer::BeginScene(Viewport& viewport)
	{
		s_RendererData.CurrentViewport = &viewport;
		s_RendererData.CameraBuffer->SetData(&viewport.FrameData.Camera, sizeof(CameraData), 0);
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::DrawMesh(const Ref<VertexArray>& mesh, const Ref<Material>& material)
	{
		material->SetShaderParameters();
		RenderCommand::DrawIndexed(mesh);
	}

	Viewport& Renderer::GetMainViewport()
	{
		Grapple_CORE_ASSERT(s_RendererData.MainViewport);
		return *s_RendererData.MainViewport;
	}

	Viewport& Renderer::GetCurrentViewport()
	{
		Grapple_CORE_ASSERT(s_RendererData.CurrentViewport);
		return *s_RendererData.CurrentViewport;
	}
}
