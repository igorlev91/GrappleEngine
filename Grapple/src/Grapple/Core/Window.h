#pragma once

#include "Grapple.h"
#include "Grapple/Core/Events.h"

#include <string>
#include <stdint.h>

namespace Grapple
{
	struct WindowProperties
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		bool IsMinimized;
		bool VSyncEnabled;
	};

	class Window
	{
	public:
		virtual const WindowProperties& GetProperties() const = 0;

		virtual void SetTitle(const std::string& title) = 0;

		virtual void SetEventCallback(const EventCallback& callback) = 0;
		virtual void SetVSync(bool vsync) = 0;

		virtual void* GetNativeWindow() = 0;

		virtual void OnUpdate() = 0;
	public:
		static Scope<Window> Create(WindowProperties& properties);
	};
}