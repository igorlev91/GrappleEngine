#include "Application.h"

namespace Grapple
{
	Application::Application()
	{
		WindowProperties properties;
		properties.Title = "Flare Engine";
		properties.Width = 1080;
		properties.Height = 720;

		m_Window = Window::Create(properties);
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