#include "ImGUILayer.h"

#include "Grapple/Core/Application.h"
#include "Grapple/Core/Core.h"
#include "Grapple/Core/Platform.h"

#ifdef Grapple_PLATFORM_WINDOWS
	#include "Grapple/Platform/Windows/WindowsWindow.h"
#endif

#include <imgui.h>
#include <imgui_internal.h>

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

namespace Grapple
{
	ImGUILayer::ImGUILayer()
		: Layer("ImGUILayer")
	{

	}

	void ImGUILayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		Application& application = Application::GetInstance();
		Ref<Window> window = application.GetWindow();

#ifdef Grapple_PLATFORM_WINDOWS
		ImGui_ImplGlfw_InitForOpenGL(((WindowsWindow*)window.get())->GetGLFWWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 410");
#endif
	}

	void ImGUILayer::OnDetach()
	{
	}

	void ImGUILayer::OnUpdate(float deltaTime)
	{
	}

	void ImGUILayer::OnEvent(Event& event)
	{
	}
	
	void ImGUILayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);
	}

	void ImGUILayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& application = Application::GetInstance();

		const WindowProperties& windowProps = application.GetWindow()->GetProperties();
		io.DisplaySize = ImVec2(windowProps.Width, windowProps.Height);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}