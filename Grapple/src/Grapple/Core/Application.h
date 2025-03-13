#pragma once

#include "Grapple/Core/Window.h"

namespace Grapple
{
	class Application
	{
	public:
		Application();

		void Run();
		void Close();
	public:
		virtual void OnUpdate() = 0;

		virtual void OnEvent(Event& event) {}
	protected:
		Ref<Window> m_Window;
	private:
		bool m_Running;
	};
}