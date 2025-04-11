#include "Application.h"

#include "Grapple/Renderer2D/Renderer2D.h"
#include "Grapple/Renderer/RenderCommand.h"

#include "Grapple/Scripting/ScriptingEngine.h"

namespace Grapple
{
	Application* Application::s_Instance = nullptr;

	Application::Application(CommandLineArguments arguments)
		: m_Running(true), m_CommandLineArguments(arguments)
	{
		s_Instance = this;

		WindowProperties properties;
		properties.Title = "Grapple Engine";
		properties.Width = 1280;
		properties.Height = 720;
		properties.CustomTitleBar = true;

		m_Window = Window::Create(properties);
		m_Window->SetEventCallback([this](Event& event)
		{
			EventDispatcher dispatcher(event);
			dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& event) -> bool
			{
				Close();
				return true;
			});

			dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) -> bool
			{
				RenderCommand::SetViewport(0, 0, event.GetWidth(), event.GetHeight());
				return true;
			});

			auto& layer = m_LayersStack.GetLayers();
			for (auto it = layer.end(); it != layer.begin();)
			{
				(--it)->get()->OnEvent(event);
				if (event.Handled)
					return;
			}
		});

		RenderCommand::Initialize();
		Renderer2D::Initialize();

		ScriptingEngine::Initialize();
	}

	Application::~Application()
	{
		ScriptingEngine::Shutdown();
		Renderer2D::Shutdown();
	}

	void Application::Run()
	{
		for (const Ref<Layer>& layer : m_LayersStack.GetLayers())
			layer->OnAttach();

		while (m_Running)
		{
			float currentTime = Time::GetTime();
			float deltaTime = currentTime - m_PreviousFrameTime;

			for (const Ref<Layer>& layer : m_LayersStack.GetLayers())
				layer->OnUpdate(deltaTime);

			for (const Ref<Layer>& layer : m_LayersStack.GetLayers())
				layer->OnImGUIRender();

			m_Window->OnUpdate();
			m_PreviousFrameTime = currentTime;
		}

		for (const Ref<Layer>& layer : m_LayersStack.GetLayers())
			layer->OnDetach();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::PushLayer(const Ref<Layer>& layer)
	{
		m_LayersStack.PushLayer(layer);
	}
	
	void Application::PushOverlay(const Ref<Layer>& layer)
	{
		m_LayersStack.PushOverlay(layer);
	}

	Application& Application::GetInstance()
	{
		Grapple_CORE_ASSERT(s_Instance != nullptr, "Application instance is not valid");
		return *s_Instance;
	}
}