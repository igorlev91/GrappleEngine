#pragma once

#include "GrappleCore/Core.h"
#include "GrapplePlatform/Window.h"

#include <GLFW/glfw3.h>

#include <Windows.h>

namespace Grapple
{
	class GrapplePLATFORM_API WindowsWindow : public Window
	{
	public:
		WindowsWindow(WindowProperties& properties);
		virtual ~WindowsWindow();
	public:
		virtual const WindowProperties& GetProperties() const override { return m_Data.Properties; }

		void SetUsesOpenGL();
		void SetUsesVulkan();
		
		virtual void Initialize() override;

		virtual void SetTitle(const std::string& title) override;
		virtual void Hide() override;
		virtual void SetMaximized(bool value) override;

		virtual void SetEventCallback(const EventCallback& callback) override { m_Data.Callback = callback; }
		virtual void SetVSync(bool vsync) override;

		virtual void* GetNativeWindow() override { return m_Window; }

		virtual void OnUpdate() override;

		GLFWwindow* GetGLFWWindow() const { return m_Window; }

		virtual void SetWindowControls(const Ref<WindowControls>& controls) { m_Data.Controls = controls; }
		virtual Ref<WindowControls> GetWindowControls() const override { return m_Data.Controls; }
		virtual glm::uvec2 GetControlsButtonSize() const override;

		virtual void SetCursorMode(CursorMode mode) override;
	private:
		static LRESULT CALLBACK CustomWindowDecorationProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
		static const wchar_t* s_WindowPropertyName;
	private:
		enum class RendererAPI
		{
			OpenGL,
			Vulkan,
		};

		struct WindowData
		{
			WindowProperties Properties;
			EventCallback Callback = nullptr;
			Ref<WindowControls> Controls = nullptr;
		};

		RendererAPI m_RendererAPI = RendererAPI::OpenGL;

		GLFWwindow* m_Window = nullptr;
		WindowData m_Data;

		void* m_OriginalProc = nullptr;
	};
}