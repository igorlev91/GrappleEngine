#pragma once

#include <Grapple/Core/Window.h>

namespace Grapple
{
	class Application
	{
	public:
		Application();

		void Run();
	public:
		virtual void OnUpdate() = 0;
	protected:
		Ref<Window> m_Window;
	};
}