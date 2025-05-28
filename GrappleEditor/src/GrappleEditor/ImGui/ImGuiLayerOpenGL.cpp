#include "ImGuiLayerOpenGL.h"

#include "GrappleCore/Profiler/Profiler.h"

#include "Grapple/Core/Application.h"
#include "GrapplePlatform/Window.h"

#include <ImGuizmo.h>

#include <GLFW/glfw3.h>

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

namespace Grapple
{
	void ImGuiLayerOpenGL::InitializeRenderer()
	{
		Application& application = Application::GetInstance();
		Ref<Window> window = application.GetWindow();

		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)window->GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayerOpenGL::ShutdownRenderer()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}

	void ImGuiLayerOpenGL::InitializeFonts()
	{
	}

	void ImGuiLayerOpenGL::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayerOpenGL::End()
	{
		Grapple_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		Application& application = Application::GetInstance();

		const WindowProperties& windowProps = application.GetWindow()->GetProperties();
		io.DisplaySize = ImVec2((float)windowProps.Size.x, (float)windowProps.Size.y);

		ImGui::EndFrame();
		ImGui::Render();
	}

	void ImGuiLayerOpenGL::RenderCurrentWindow()
	{
		Grapple_PROFILE_FUNCTION();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayerOpenGL::UpdateWindows()
	{
		Grapple_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* currentContext = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(currentContext);
		}
	}
}
