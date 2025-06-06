#include "ViewportWindow.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/Passes/BlitPass.h"

#include "Grapple/Renderer/PostProcessing/SSAO.h"
#include "Grapple/Renderer/PostProcessing/ToneMapping.h"
#include "Grapple/Renderer/PostProcessing/Vignette.h"
#include "Grapple/Renderer/PostProcessing/AtmospherePass.h"

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

			OnClear();

			Renderer::BeginScene(m_Viewport);
			scene->OnRender(m_Viewport);
			Renderer::EndScene();

			if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
			{
				Ref<VulkanCommandBuffer> commandBuffer = VulkanContext::GetInstance().GetPrimaryCommandBuffer();
				Ref<VulkanFrameBuffer> target = As<VulkanFrameBuffer>(m_Viewport.RenderTarget);

				commandBuffer->TransitionImageLayout(As<VulkanTexture>(m_Viewport.ColorTexture)->GetImageHandle(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
				commandBuffer->TransitionImageLayout(As<VulkanTexture>(m_Viewport.NormalsTexture)->GetImageHandle(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
				commandBuffer->TransitionDepthImageLayout(As<VulkanTexture>(m_Viewport.DepthTexture)->GetImageHandle(), true, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			}
		}
	}

	void ViewportWindow::PrepareViewport()
	{
		if (GetScene()->GetPostProcessingManager().IsDirty())
			m_ShouldRebuildRenderGraph = true;

		if (m_Viewport.GetSize() != glm::ivec2(0))
		{
			const FrameBufferSpecifications frameBufferSpecs = m_Viewport.RenderTarget->GetSpecifications();

			if (frameBufferSpecs.Width != m_Viewport.GetSize().x || frameBufferSpecs.Height != m_Viewport.GetSize().y)
			{
				m_Viewport.RenderTarget->Resize(m_Viewport.GetSize().x, m_Viewport.GetSize().y);
				OnViewportChanged();
			}

			if (m_ShouldRebuildRenderGraph)
			{
				BuildRenderGraph();
				m_ShouldRebuildRenderGraph = false;
			}
		}
	}

	void ViewportWindow::RequestRenderGraphRebuild()
	{
		m_ShouldRebuildRenderGraph = true;

		Ref<Scene> scene = GetScene();

		if (scene != nullptr)
		{
			scene->GetPostProcessingManager().MarkAsDirty();
		}
	}

	void ViewportWindow::BeginImGui()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		m_IsVisible = ImGui::Begin(m_Name.c_str(), &ShowWindow, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

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

		if (changed)
		{
			bool shouldCreateFrameBuffers = m_Viewport.GetSize() == glm::ivec2(0);
			m_Viewport.Resize(viewportPosition, viewportSize);

			if (shouldCreateFrameBuffers)
				CreateFrameBuffer();

			OnViewportChanged();
		}
	}

	void ViewportWindow::RenderViewportBuffer(const Ref<Texture>& texture)
	{
		ImVec2 windowSize = ImGui::GetContentRegionAvail();

		if (m_Viewport.RenderTarget != nullptr)
		{
			const FrameBufferSpecifications frameBufferSpecs = m_Viewport.RenderTarget->GetSpecifications();
			ImVec2 imageSize = ImVec2((float)frameBufferSpecs.Width, (float)frameBufferSpecs.Height);
			ImGui::Image(ImGuiLayer::GetId(texture), windowSize, ImVec2(0, 1), ImVec2(1, 0));
		}
	}

	void ViewportWindow::EndImGui()
	{
		ImGui::End();
		ImGui::PopStyleVar(); // Pop window padding
	}

	void ViewportWindow::CreateFrameBuffer()
	{
		TextureSpecifications specifications{};
		specifications.Width = m_Viewport.GetSize().x;
		specifications.Height = m_Viewport.GetSize().y;
		specifications.Wrap = TextureWrap::Clamp;
		specifications.Filtering = TextureFiltering::Closest;
		specifications.GenerateMipMaps = false;
		specifications.Usage = TextureUsage::Sampling | TextureUsage::RenderTarget;

		TextureSpecifications colorSpecifications = specifications;
		colorSpecifications.Format = TextureFormat::R11G11B10;

		TextureSpecifications normalsSpecifications = specifications;
		normalsSpecifications.Format = TextureFormat::RGB8;

		TextureSpecifications depthSpecifications = specifications;
		depthSpecifications.Format = TextureFormat::Depth24Stencil8;

		m_Viewport.ColorTexture = Texture::Create(colorSpecifications);
		m_Viewport.NormalsTexture = Texture::Create(normalsSpecifications);
		m_Viewport.DepthTexture = Texture::Create(depthSpecifications);

		Ref<Texture> attachmentTextures[] = { m_Viewport.ColorTexture, m_Viewport.NormalsTexture, m_Viewport.DepthTexture };

		m_Viewport.RenderTarget = FrameBuffer::Create(Span(attachmentTextures, 3));

		ExternalRenderGraphResource colorTextureResource{};
		colorTextureResource.InitialLayout = ImageLayout::AttachmentOutput;
		colorTextureResource.FinalLayout = ImageLayout::AttachmentOutput;
		colorTextureResource.TextureHandle = m_Viewport.ColorTexture;

		ExternalRenderGraphResource normalsTextureResource{};
		normalsTextureResource.InitialLayout = ImageLayout::AttachmentOutput;
		normalsTextureResource.FinalLayout = ImageLayout::AttachmentOutput;
		normalsTextureResource.TextureHandle = m_Viewport.NormalsTexture;

		ExternalRenderGraphResource depthTextureResource{};
		depthTextureResource.InitialLayout = ImageLayout::AttachmentOutput;
		depthTextureResource.FinalLayout = ImageLayout::AttachmentOutput;
		depthTextureResource.TextureHandle = m_Viewport.DepthTexture;

		m_Viewport.Graph.AddExternalResource(colorTextureResource);
		m_Viewport.Graph.AddExternalResource(normalsTextureResource);
		m_Viewport.Graph.AddExternalResource(depthTextureResource);
	}

	void ViewportWindow::OnClear()
	{
		Ref<CommandBuffer> commandBuffer = GraphicsContext::GetInstance().GetCommandBuffer();
		commandBuffer->ClearColor(m_Viewport.ColorTexture, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		commandBuffer->ClearColor(m_Viewport.NormalsTexture, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		commandBuffer->ClearDepth(m_Viewport.DepthTexture, 1.0f);
	}

	void ViewportWindow::OnViewportChanged()
	{
		m_ShouldRebuildRenderGraph = true;
	}

	void ViewportWindow::BuildRenderGraph()
	{
		m_Viewport.Graph.Clear();

		Ref<Scene> scene = GetScene();

		Renderer2D::ConfigurePasses(m_Viewport);

		if (scene)
		{
			PostProcessingManager& postProcessing = scene->GetPostProcessingManager();
			postProcessing.MarkAsDirty(); // HACK
			postProcessing.RegisterRenderPasses(m_Viewport.Graph, m_Viewport);
		}

		m_Viewport.Graph.Build();
	}

	void ViewportWindow::OnAttach()
	{
	}

	void ViewportWindow::OnRenderImGui()
	{
		if (!ShowWindow)
			return;

		BeginImGui();
		RenderViewportBuffer(m_Viewport.ColorTexture);
		EndImGui();
	}
}
