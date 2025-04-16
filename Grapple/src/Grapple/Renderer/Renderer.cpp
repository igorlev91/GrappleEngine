#include "Renderer.h"

namespace Grapple
{
	struct RendererData
	{
		Viewport* MainViewport;
		Viewport* CurrentViewport;
	};

	RendererData s_RendererData;

	void Renderer::Initialize()
	{
		s_RendererData.MainViewport = nullptr;
		s_RendererData.CurrentViewport = nullptr;
	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::SetMainViewport(Viewport& viewport)
	{
		s_RendererData.MainViewport = &viewport;
	}

	void Renderer::SetCurrentViewport(Viewport& viewport)
	{
		s_RendererData.CurrentViewport = &viewport;
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
