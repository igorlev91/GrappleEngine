#include "Window.h"

#include "GrapplePlatform/Window.h"
#include "GrapplePlatform/Windows/WindowsWindow.h"

namespace Grapple
{
	Scope<Window> Window::Create(WindowProperties& properties)
	{
#ifdef Grapple_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(properties);
#endif
	}
}