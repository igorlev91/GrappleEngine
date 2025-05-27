#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/Core/LayerStack.h"
#include "Grapple/Core/CommandLineArguments.h"

#include "Grapple/Renderer/GraphicsContext.h"

#include "GrapplePlatform/Window.h"

namespace Grapple
{
	class Grapple_API Application
	{
	public:
		Application(CommandLineArguments arguments);
		virtual ~Application();

		void Run();
		void Close();

		void PushLayer(const Ref<Layer>& layer);
		void PushOverlay(const Ref<Layer>& layer);

		Ref<Window> GetWindow() const { return m_Window; }
		CommandLineArguments GetCommandLineArguments() const { return m_CommandLineArguments; }

		static Application& GetInstance();
	protected:
		Ref<Window> m_Window;
		CommandLineArguments m_CommandLineArguments;
	private:
		LayerStack m_LayersStack;

		bool m_Running;
		float m_PreviousFrameTime;
	};
}