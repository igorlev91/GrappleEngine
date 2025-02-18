#include "Application.h"

#include <Grapple/Renderer/RenderCommand.h>

namespace Grapple
{
	Application::Application()
	{
		WindowProperties properties;
		properties.Title = "Grapple Engine";
		properties.Width = 1080;
		properties.Height = 720;

		m_Window = Window::Create(properties);

		RenderCommand::Initialize();
	}

	void Application::Run()
	{
		while (!m_Window->ShouldClose())
		{
			OnUpdate();
			m_Window->OnUpdate();
		}
	}
}