#include "Application.h"

#include "Grapple/Core/Time.h"

#include "GrappleCore/Profiler/Profiler.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer2D/Renderer2D.h"
#include "Grapple/Renderer/DebugRenderer.h"
#include "Grapple/Renderer/RenderCommand.h"

#include "Grapple/Scripting/ScriptingEngine.h"
#include "Grapple/Input/InputManager.h"

#include "GrapplePlatform/Event.h"
#include "GrapplePlatform/Events.h"

#include "GrapplePlatform/Platform.h"

namespace Grapple
{
	Application* s_Instance = nullptr;

	Application::Application(CommandLineArguments arguments)
		: m_Running(true), m_CommandLineArguments(arguments)
	{
		s_Instance = this;

		WindowProperties properties;
		properties.Title = "Grapple Engine";
		properties.Size = glm::uvec2(1280, 720);
		properties.CustomTitleBar = true;

		m_Window = Window::Create(properties);
		m_GraphicsContext = GraphicsContext::Create(m_Window->GetNativeWindow());
		m_GraphicsContext->Initialize();
		
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
	}

	Application::~Application()
	{
		ScriptingEngine::Shutdown();
		Renderer2D::Shutdown();
		DebugRenderer::Shutdown();
		Renderer::Shutdown();
	}

	void Application::Run()
	{
		InputManager::Initialize();

		RenderCommand::Initialize();
		Renderer::Initialize();
		Renderer2D::Initialize();
		DebugRenderer::Initialize();

		ScriptingEngine::Initialize();

		RenderCommand::SetLineWidth(1.2f);

		for (const Ref<Layer>& layer : m_LayersStack.GetLayers())
			layer->OnAttach();

		while (m_Running)
		{
			Profiler::BeginFrame();
			Grapple_PROFILE_BEGIN_FRAME("Main");

			{
				Grapple_PROFILE_SCOPE("Application::Update");

				float currentTime = Platform::GetTime();
				float deltaTime = currentTime - m_PreviousFrameTime;

				Time::UpdateDeltaTime();

				InputManager::Update();
				m_Window->OnUpdate();

				{
					Grapple_PROFILE_SCOPE("Layers::OnUpdate");
					for (const Ref<Layer>& layer : m_LayersStack.GetLayers())
						layer->OnUpdate(deltaTime);
				}

				{
					Grapple_PROFILE_SCOPE("Layers::OnImGui");
					for (const Ref<Layer>& layer : m_LayersStack.GetLayers())
						layer->OnImGUIRender();
				}

				{
					Grapple_PROFILE_SCOPE("SwapBuffers");
					m_GraphicsContext->SwapBuffers();
				}

				m_PreviousFrameTime = currentTime;
			}

			Grapple_PROFILE_END_FRAME("Main");
			Profiler::EndFrame();
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