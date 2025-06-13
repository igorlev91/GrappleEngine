#pragma once

#include "GrappleCore/Core.h"

#include "GrapplePlatform/Event.h"
#include "GrapplePlatform/Events.h"
#include "GrapplePlatform/WindowControls.h"

#include <glm/glm.hpp>

#include <string>
#include <stdint.h>

namespace Grapple
{
	enum class FullscreenMode
	{
		None,
		Fullscreen,
		ExclusiveFullscreen,
	};

	struct WindowProperties
	{
		std::string Title;
		glm::uvec2 Size;
		glm::ivec2 Position;
		bool IsMinimized = false;
		bool IsFocused = false;
		bool IsMaximized = false;
		bool CustomTitleBar = false;
		bool VSyncEnabled = true;
		FullscreenMode FullscreenMode = FullscreenMode::None;
	};

	enum class CursorMode
	{
		Normal,
		Hidden,
		Captured,
		Disabled,
	};

	class GrapplePLATFORM_API Window
	{
	public:
		virtual ~Window() {}
		virtual const WindowProperties& GetProperties() const = 0;

		virtual void Initialize() = 0;

		virtual void SetTitle(const std::string& title) = 0;
		virtual void Hide() = 0;
		virtual void SetMaximized(bool value) = 0;

		virtual void SetFullscreenMode(FullscreenMode mode) = 0;

		virtual void SetEventCallback(const EventCallback& callback) = 0;
		virtual void SetVSync(bool vsync) = 0;

		virtual void* GetNativeWindow() = 0;

		virtual void OnUpdate() = 0;

		virtual void SetWindowControls(const Ref<WindowControls>& controls) = 0;
		virtual Ref<WindowControls> GetWindowControls() const = 0;
		virtual glm::uvec2 GetControlsButtonSize() const = 0;
		
		virtual void SetCursorMode(CursorMode mode) = 0;
	public:
		static Scope<Window> Create(WindowProperties& properties);
	};
}