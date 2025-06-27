#include "ViewportWindow.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/Passes/BlitPass.h"

#include "Grapple/DebugRenderer/DebugRenderer.h"

#include "Grapple/Platform/Vulkan/VulkanContext.h"

#include "Grapple/Scene/Scene.h"

#include "Grapple/Core/Application.h"
#include "GrappleCore/Profiler/Profiler.h"

#include "GrappleEditor/ImGui/ImGuiLayer.h"

namespace Grapple
{
	ViewportWindow::ViewportWindow(std::string_view name)
		: m_Name(name),
		m_IsFocused(false),
		m_IsVisible(true),
		m_PreviousFocusState(false),
		m_IsHovered(false),
		ShowWindow(true),
		m_RelativeMousePosition(glm::ivec2(0)),
		m_ViewportOffset(glm::uvec2(0))
	{
	}

	void ViewportWindow::OnRenderViewport()
	{
		Grapple_PROFILE_FUNCTION();

		if (!ShowWindow || !m_IsVisible)
			return;

		Ref<Scene> scene = GetScene();
		if (scene == nullptr)
			return;

		PrepareViewport();

		if (m_Viewport.GetSize() != glm::ivec2(0))
		{
			scene->OnBeforeRender(m_Viewport);

			Renderer::BeginScene(m_Viewport);

			OnClear();

			scene->OnRender(m_Viewport);
			Renderer::EndScene();
		}
	}

	void ViewportWindow::OnAddRenderPasses()
	{
	}

	void ViewportWindow::SetMaximized(bool maximized)
	{
		m_Maximized = maximized;
	}

	void ViewportWindow::PrepareViewport()
	{
		if (GetScene()->GetPostProcessingManager().IsDirty())
			m_Viewport.Graph.SetNeedsRebuilding();

		if (Renderer::RequiresRenderGraphRebuild())
			m_Viewport.Graph.SetNeedsRebuilding();

		if (m_Viewport.GetSize() != glm::ivec2(0))
		{
			if (m_Viewport.Graph.NeedsRebuilding())
			{
				BuildRenderGraph();
			}
		}
	}

	void ViewportWindow::BeginImGui()
	{
		Grapple_PROFILE_FUNCTION();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

		if (m_Maximized)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		m_IsVisible = ImGui::Begin(m_Name.c_str(), &ShowWindow, windowFlags);

		// HACK: If window's title bar isn't hovered, disable window moving by draging anywhere inside the window.
		//       When the window wasn't docket it used to interfere with the camera controller and guizmos,
		//       as moving a cemera or using a guizmo was also moving a window.
		if (!ImGui::IsWindowDocked())
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			ImRect titleBarRect = window->TitleBarRect();
			bool titleBarIsHovered = ImGui::IsMouseHoveringRect(titleBarRect.Min, titleBarRect.Max, false);

			if (!titleBarIsHovered)
			{
				window->Flags |= ImGuiWindowFlags_NoMove;
			}
		}

		if (m_WindowFocusRequested)
		{
			ImGui::FocusWindow(ImGui::GetCurrentWindow());
			m_WindowFocusRequested = false;
		}

		m_PreviousFocusState = m_IsFocused;

		m_IsHovered = ImGui::IsWindowHovered();
		m_IsFocused = ImGui::IsWindowFocused();

		if (!m_IsVisible)
			return;

		ImGuiIO& io = ImGui::GetIO();

		Ref<Window> window = Application::GetInstance().GetWindow();

		ImVec2 cursorPosition = ImGui::GetCursorPos();
		ImVec2 windowPosition = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		glm::ivec2 newViewportSize = glm::u32vec2((int32_t)windowSize.x, (int32_t)windowSize.y);
		m_ViewportOffset = glm::ivec2(cursorPosition.x, cursorPosition.y);

		m_RelativeMousePosition = glm::ivec2(
			(int32_t)(io.MousePos.x - windowPosition.x),
			(int32_t)(io.MousePos.y - windowPosition.y)) - m_ViewportOffset;

		m_RelativeMousePosition.y = newViewportSize.y - m_RelativeMousePosition.y;

		bool changed = false;
		glm::ivec2 viewportSize = m_Viewport.GetSize();
		glm::ivec2 viewportPosition = m_Viewport.GetPosition();

		if (m_Viewport.GetSize() == glm::ivec2(0))
		{
			viewportSize = newViewportSize;
			changed = true;
		}
		else if (newViewportSize != m_Viewport.GetSize())
		{
			viewportSize = newViewportSize;
			changed = true;
		}

		glm::ivec2 position = glm::ivec2(windowPosition.x, windowPosition.y) + m_ViewportOffset - (glm::ivec2)window->GetProperties().Position;
		if (position != viewportPosition)
		{
			viewportPosition = position;
			changed = true;
		}

		bool shouldResizeViewport = viewportSize != m_Viewport.GetSize();
		if (shouldResizeViewport)
		{
			bool shouldCreateFrameBuffers = m_Viewport.GetSize() == glm::ivec2(0);
			m_Viewport.Resize(viewportPosition, viewportSize);
		}

		if (changed)
		{
			OnViewportChanged();
		}
	}

	void ViewportWindow::RenderViewportBuffer(const Ref<Texture>& texture)
	{
		Grapple_PROFILE_FUNCTION();
		ImVec2 windowSize = ImGui::GetContentRegionAvail();

		ImVec2 imageSize = ImVec2((float)m_Viewport.GetSize().x, (float)m_Viewport.GetSize().y);
		ImGui::Image(ImGuiLayer::GetId(texture), windowSize, ImVec2(0, 1), ImVec2(1, 0));
	}

	void ViewportWindow::EndImGui()
	{
		Grapple_PROFILE_FUNCTION();
		ImGui::End();
		ImGui::PopStyleVar(2); // Pop window padding & border size
	}

	void ViewportWindow::OnClear()
	{
		Grapple_PROFILE_FUNCTION();
		Ref<CommandBuffer> commandBuffer = GraphicsContext::GetInstance().GetCommandBuffer();

		const auto& resourceManager = m_Viewport.Graph.GetResourceManager();

		commandBuffer->ClearColor(resourceManager.GetTexture(m_Viewport.ColorTextureId), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		commandBuffer->ClearColor(resourceManager.GetTexture(m_Viewport.NormalsTextureId), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		commandBuffer->ClearDepth(resourceManager.GetTexture(m_Viewport.DepthTextureId), 1.0f);
	}

	void ViewportWindow::OnViewportChanged()
	{
	}

	void ViewportWindow::BuildRenderGraph()
	{
		Grapple_PROFILE_FUNCTION();
		Ref<Scene> scene = GetScene();

		if (scene != nullptr)
			scene->GetPostProcessingManager().MarkAsDirty();

		m_Viewport.Graph.Clear();
		m_Viewport.OnBuildRenderGraph();

		Renderer::ConfigurePasses(m_Viewport);
		Renderer2D::ConfigurePasses(m_Viewport);

		if (scene && m_Viewport.IsPostProcessingEnabled())
		{
			PostProcessingManager& postProcessing = scene->GetPostProcessingManager();
			postProcessing.MarkAsDirty(); // HACK
			postProcessing.RegisterRenderPasses(m_Viewport.Graph, m_Viewport);
		}

		OnAddRenderPasses();
		DebugRenderer::ConfigurePasses(m_Viewport);

		m_Viewport.Graph.Build();
	}

	void ViewportWindow::OnAttach()
	{
		Grapple_PROFILE_FUNCTION();
		m_Viewport.Graph.SetNeedsRebuilding();
	}

	void ViewportWindow::OnRenderImGui()
	{
		Grapple_PROFILE_FUNCTION();
		if (!ShowWindow)
			return;

		BeginImGui();

		if (m_Viewport.ColorTextureId != RenderGraphTextureId())
		{
			RenderViewportBuffer(m_Viewport.Graph.GetTexture(m_Viewport.ColorTextureId));
		}

		EndImGui();
	}
}
