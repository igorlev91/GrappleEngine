#include "SceneViewportWindow.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/DebugRenderer.h"
#include "Grapple/Renderer/ShaderLibrary.h"

#include "Grapple/Platform/Vulkan/VulkanContext.h"

#include "Grapple/Scene/Components.h"
#include "Grapple/Scene/Scene.h"
#include "Grapple/Scene/Prefab.h"

#include "Grapple/Math/Math.h"

#include "Grapple/Input/InputManager.h"

#include "GrappleEditor/AssetManager/EditorAssetManager.h"
#include "GrappleEditor/ImGui/ImGuiLayer.h"
#include "GrappleEditor/EditorLayer.h"
#include "GrappleEditor/UI/EditorGUI.h"

#include "GrapplePlatform//Events.h"

#include <ImGuizmo.h>

namespace Grapple
{
	struct GridPropertyIndices
	{
		uint32_t Offset = UINT32_MAX;
		uint32_t Scale = UINT32_MAX;
		uint32_t Thickness = UINT32_MAX;
		uint32_t CellScale = UINT32_MAX;
		uint32_t Color = UINT32_MAX;
		uint32_t FallOffThreshold = UINT32_MAX;
	};

	static GridPropertyIndices s_GridPropertyIndices;

	SceneViewportWindow::SceneViewportWindow(EditorCamera& camera, std::string_view name)
		: ViewportWindow(name),
		m_Camera(camera),
		m_Overlay(ViewportOverlay::Default),
		m_IsToolbarHovered(false),
		m_CameraController(m_Camera),
		m_Guizmo(GuizmoMode::None) {}

	void SceneViewportWindow::OnAttach()
	{
		AssetHandle gridShaderHandle = ShaderLibrary::FindShader("SceneViewGrid").value_or(NULL_ASSET_HANDLE);
		if (AssetManager::IsAssetHandleValid(gridShaderHandle))
		{
			if (RendererAPI::GetAPI() != RendererAPI::API::Vulkan)
			{
				m_GridMaterial = Material::Create(AssetManager::GetAsset<Shader>(gridShaderHandle));

				Ref<Shader> gridShader = m_GridMaterial->GetShader();
				s_GridPropertyIndices.Color = gridShader->GetPropertyIndex("u_Data.Color").value_or(UINT32_MAX);
				s_GridPropertyIndices.Offset = gridShader->GetPropertyIndex("u_Data.Offset").value_or(UINT32_MAX);
				s_GridPropertyIndices.Scale = gridShader->GetPropertyIndex("u_Data.GridScale").value_or(UINT32_MAX);
				s_GridPropertyIndices.Thickness = gridShader->GetPropertyIndex("u_Data.Thickness").value_or(UINT32_MAX);
				s_GridPropertyIndices.CellScale = gridShader->GetPropertyIndex("u_Data.CellScale").value_or(UINT32_MAX);
				s_GridPropertyIndices.FallOffThreshold = gridShader->GetPropertyIndex("u_Data.FallOffThreshold").value_or(UINT32_MAX);
			}
		}
		else
			Grapple_CORE_ERROR("Failed to load scene view grid shader");

		AssetHandle selectionOutlineShader = ShaderLibrary::FindShader("SelectionOutline").value_or(NULL_ASSET_HANDLE);
		if (AssetManager::IsAssetHandleValid(selectionOutlineShader))
		{
			if (false)
			{
				Ref<Shader> shader = AssetManager::GetAsset<Shader>(selectionOutlineShader);
				m_SelectionOutlineMaterial = Material::Create(shader);

				ImVec4 primaryColor = ImGuiTheme::Primary;
				glm::vec4 selectionColor = glm::vec4(primaryColor.x, primaryColor.y, primaryColor.z, 1.0f);

				std::optional<uint32_t> colorProperty = shader->GetPropertyIndex("u_Outline.Color");

				if (colorProperty)
					m_SelectionOutlineMaterial->WritePropertyValue(*colorProperty, selectionColor);
			}
		}
		else
			Grapple_CORE_ERROR("Failed to load selection outline shader");
	}

	void SceneViewportWindow::OnRenderViewport()
	{
		Grapple_PROFILE_FUNCTION();

		Ref<Scene> scene = GetScene();
		if (scene == nullptr || !ShowWindow || !m_IsVisible)
			return;

		PrepareViewport();

		if (m_Viewport.GetSize() == glm::ivec2(0))
			return;

		std::optional<SystemGroupId> debugRenderingGroup = scene->GetECSWorld().GetSystemsManager().FindGroup("Debug Rendering");
		scene->OnBeforeRender(m_Viewport);

		// Override with EditorCamera's data
		m_Viewport.FrameData.Camera.SetViewAndProjection(
			m_Camera.GetProjectionMatrix(),
			m_Camera.GetViewMatrix());

		m_Viewport.FrameData.Camera.Position = m_Camera.GetPosition();
		m_Viewport.FrameData.Camera.ViewDirection = m_Camera.GetViewDirection();

		m_Viewport.FrameData.Camera.Near = m_Camera.GetSettings().Near;
		m_Viewport.FrameData.Camera.Far = m_Camera.GetSettings().Far;

		m_Viewport.FrameData.Camera.FOV = m_Camera.GetSettings().FOV;

		Renderer::BeginScene(m_Viewport);
		OnClear();

		scene->OnRender(m_Viewport);

		//RenderGrid();

		std::optional<Entity> selectedEntity = EditorLayer::GetInstance().Selection.TryGetEntity();
		if (debugRenderingGroup.has_value())
		{
			DebugRenderer::Begin();

			scene->GetECSWorld().GetSystemsManager().ExecuteGroup(debugRenderingGroup.value());

			// Draw bouding box for decal projectors
			if (selectedEntity)
			{
				const Decal* decal = scene->GetECSWorld().TryGetEntityComponent<const Decal>(*selectedEntity);
				const TransformComponent* transform = scene->GetECSWorld().TryGetEntityComponent<const TransformComponent>(*selectedEntity);
				if (decal && transform)
				{
					glm::vec3 cubeCorners[] =
					{
						glm::vec3(-0.5f, -0.5f, -0.5f),
						glm::vec3(+0.5f, -0.5f, -0.5f),
						glm::vec3(-0.5f, +0.5f, -0.5f),
						glm::vec3(+0.5f, +0.5f, -0.5f),

						glm::vec3(-0.5f, -0.5f, +0.5f),
						glm::vec3(+0.5f, -0.5f, +0.5f),
						glm::vec3(-0.5f, +0.5f, +0.5f),
						glm::vec3(+0.5f, +0.5f, +0.5f),
					};

					glm::mat4 transformationMatrix = transform->GetTransformationMatrix();
					for (size_t i = 0; i < 8; i++)
						cubeCorners[i] = transformationMatrix * glm::vec4(cubeCorners[i], 1.0f);

					DebugRenderer::DrawWireBox(cubeCorners);
				}
			}

			DebugRenderer::End();
		}

		if (selectedEntity && m_SelectionOutlineMaterial && false)
		{
			Ref<Shader> shader = m_SelectionOutlineMaterial->GetShader();
			std::optional<uint32_t> idPropertyIndex = shader->GetPropertyIndex("u_Outline.SelectedId");
			std::optional<uint32_t> thicknessPropertyIndex = shader->GetPropertyIndex("u_Outline.Thickness");

			if (idPropertyIndex && thicknessPropertyIndex)
			{
				m_SelectionOutlineMaterial->WritePropertyValue<int32_t>(
					*idPropertyIndex,
					(int32_t)selectedEntity->GetIndex());

				m_SelectionOutlineMaterial->WritePropertyValue(
					*thicknessPropertyIndex,
					glm::vec2(4.0f) / (glm::vec2)m_Viewport.GetSize() / 2.0f);

				// TODO: also implement
			}
		}

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

	void SceneViewportWindow::OnViewportChanged()
	{
		Grapple_PROFILE_FUNCTION();
		ViewportWindow::OnViewportChanged();
		m_Camera.OnViewportChanged(m_Viewport.GetSize(), m_Viewport.GetPosition());
	}

	void SceneViewportWindow::OnRenderImGui()
	{
		Grapple_PROFILE_FUNCTION();
		if (!ShowWindow)
			return;

		BeginImGui();

		if (m_IsVisible)
		{
			if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
				ImGui::SetWindowFocus();

			RenderWindowContents();
		}

		m_CameraController.Update(m_RelativeMousePosition);

		if (m_IsFocused)
		{
			EditorLayer& editorLayer = EditorLayer::GetInstance();
			GuizmoMode guizmoMode = m_Guizmo;

			if (ImGui::IsKeyPressed(ImGuiKey_Escape))
				guizmoMode = GuizmoMode::None;
			if (ImGui::IsKeyPressed(ImGuiKey_G))
				guizmoMode = GuizmoMode::Translate;
			if (ImGui::IsKeyPressed(ImGuiKey_R))
				guizmoMode = GuizmoMode::Rotate;
			if (ImGui::IsKeyPressed(ImGuiKey_S))
				guizmoMode = GuizmoMode::Scale;

			if (ImGui::IsKeyPressed(ImGuiKey_F))
			{
				const auto& editorSelection = EditorLayer::GetInstance().Selection;
				if (editorSelection.GetType() == EditorSelectionType::Entity)
				{
					const World& world = GetScene()->GetECSWorld();
					const TransformComponent* transform = world.TryGetEntityComponent<TransformComponent>(editorSelection.GetEntity());

					if (transform)
						m_Camera.SetRotationOrigin(transform->Position);
				}
			}
				
			m_Guizmo = guizmoMode;
		}

		EndImGui();
	}

	void SceneViewportWindow::OnEvent(Event& event)
	{
	}

	void SceneViewportWindow::CreateFrameBuffer()
	{
		Grapple_PROFILE_FUNCTION();
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

		m_Viewport.ColorTexture->SetDebugName("Color");
		m_Viewport.NormalsTexture->SetDebugName("Normals");
		m_Viewport.DepthTexture->SetDebugName("Depth");

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

	void SceneViewportWindow::OnClear()
	{
		Grapple_PROFILE_FUNCTION();
		Ref<CommandBuffer> commandBuffer = GraphicsContext::GetInstance().GetCommandBuffer();
		commandBuffer->ClearColor(m_Viewport.ColorTexture, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		commandBuffer->ClearColor(m_Viewport.NormalsTexture, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		commandBuffer->ClearDepth(m_Viewport.DepthTexture, 1.0f);
	}

	void SceneViewportWindow::RenderWindowContents()
	{
		Grapple_PROFILE_FUNCTION();
		if (GetScene() == nullptr)
			return;

		switch (m_Overlay)
		{
		case ViewportOverlay::Default:
			RenderViewportBuffer(m_Viewport.ColorTexture);
			break;
		case ViewportOverlay::Normal:
			RenderViewportBuffer(m_Viewport.NormalsTexture);
			break;
		case ViewportOverlay::Depth:
			RenderViewportBuffer(m_Viewport.DepthTexture);
			break;
		}

		World& world = GetScene()->GetECSWorld();
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ASSET_PAYLOAD_NAME))
			{
				std::optional<Entity> entity = GetEntityUnderCursor();

				HandleAssetDragAndDrop(*(AssetHandle*)payload->Data);

				ImGui::EndDragDropTarget();
			}
		}

		// Render scene toolbar
		RenderToolBar();

		m_IsToolbarHovered = ImGui::IsAnyItemHovered();

		ImGuiIO& io = ImGui::GetIO();

		const EditorSelection& selection = EditorLayer::GetInstance().Selection;

		bool showGuizmo = m_Guizmo != GuizmoMode::None;
		bool hasSelection = selection.GetType() == EditorSelectionType::Entity && world.IsEntityAlive(selection.GetEntity());
		if (showGuizmo && hasSelection)
		{
			Entity selectedEntity = selection.GetEntity();
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImVec2 windowPosition = ImGui::GetWindowPos();
			ImGuizmo::SetRect(windowPosition.x + m_ViewportOffset.x,
				windowPosition.y + m_ViewportOffset.y,
				(float)m_Viewport.GetSize().x,
				(float)m_Viewport.GetSize().y);

			TransformComponent* transform = world.TryGetEntityComponent<TransformComponent>(selectedEntity);
			if (transform)
			{
				glm::mat4 transformationMatrix = transform->GetTransformationMatrix();

				// TODO: move snap values to editor settings
				float snapValue = 0.5f;

				ImGuizmo::OPERATION operation = (ImGuizmo::OPERATION)-1;
				switch (m_Guizmo)
				{
				case GuizmoMode::Translate:
					operation = ImGuizmo::TRANSLATE;
					break;
				case GuizmoMode::Rotate:
					snapValue = 5.0f;
					operation = ImGuizmo::ROTATE;
					break;
				case GuizmoMode::Scale:
					operation = ImGuizmo::SCALE;
					break;
				default:
					Grapple_CORE_ASSERT("Unhandled Gizmo type");
				}

				ImGuizmo::MODE mode = ImGuizmo::WORLD;

				bool snappingEnabled = InputManager::IsKeyHeld(KeyCode::LeftControl) || InputManager::IsKeyHeld(KeyCode::RightControl);

				if (ImGuizmo::Manipulate(
					glm::value_ptr(m_Camera.GetViewMatrix()),
					glm::value_ptr(m_Camera.GetProjectionMatrix()),
					operation, mode,
					glm::value_ptr(transformationMatrix),
					nullptr, snappingEnabled ? &snapValue : nullptr))
				{
					Math::DecomposeTransform(transformationMatrix,
						transform->Position,
						transform->Rotation,
						transform->Scale);

					transform->Rotation = glm::degrees(transform->Rotation);
				}
			}
		}

		if (!ImGuizmo::IsUsingAny() && !m_IsToolbarHovered)
		{
			if (io.MouseClicked[ImGuiMouseButton_Left] && m_Viewport.RenderTarget != nullptr && m_IsHovered && m_RelativeMousePosition.x >= 0 && m_RelativeMousePosition.y >= 0)
			{
				std::optional<Entity> entity = GetEntityUnderCursor();

				if (entity)
				{
					EditorLayer::GetInstance().Selection.SetEntity(*entity);
				}
			}
		}
	}

	static bool GuizmoButton(const char* text, bool active)
	{
		bool result = false;
		ImDrawList* drawList = ImGui::GetCurrentWindow()->DrawList;
		ImGuiStyle& style = ImGui::GetStyle();

		if (ImGui::InvisibleButton(text, ImVec2(30, style.FramePadding.y * 2 + ImGui::GetFontSize())))
			result = true;

		ImRect buttonRect = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax() };
		ImU32 buttonColor = 0;

		ImVec2 textSize = ImGui::CalcTextSize(text, text + 1);
		ImVec2 textPosition = buttonRect.Min + buttonRect.GetSize() / 2.0f - (textSize / 2.0f);

		if (ImGui::IsItemHovered())
			buttonColor = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_ButtonHovered]);
		if (active)
			buttonColor = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_ButtonActive]);

		if (buttonColor != 0)
			drawList->AddRectFilled(buttonRect.Min, buttonRect.Max, buttonColor, style.FrameRounding);

		drawList->AddText(textPosition, ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]), text, text + 1);

		return result;
	}

	void SceneViewportWindow::RenderToolBar()
	{
		Grapple_PROFILE_FUNCTION();
		ImDrawList* drawList = ImGui::GetCurrentWindow()->DrawList;
		ImVec2 initialCursorPosition = ImGui::GetCursorPos();

		const ImGuiStyle& style = ImGui::GetStyle();
		ImRect viewportImageRect = { ImGui::GetItemRectMin() + style.FramePadding * ImVec2(3, 1), ImGui::GetItemRectMax() };

		ImGui::SetCursorPos(ImVec2((float)m_ViewportOffset.x, (float)m_ViewportOffset.y) + style.FramePadding * ImVec2(3, 1));

		ImGui::PushItemWidth(100);
		ImGui::PushID("Overlay");

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, style.FramePadding); // Window Padding
		const char* overlayName = nullptr;
		switch (m_Overlay)
		{
		case ViewportOverlay::Default:
			overlayName = "Default";
			break;
		case ViewportOverlay::Normal:
			overlayName = "Normal";
			break;
		case ViewportOverlay::Depth:
			overlayName = "Depth";
			break;
		}

		if (ImGui::BeginCombo("", overlayName))
		{
			if (ImGui::MenuItem("Default"))
				m_Overlay = ViewportOverlay::Default;
			if (ImGui::MenuItem("Normal"))
				m_Overlay = ViewportOverlay::Normal;
			if (ImGui::MenuItem("Depth"))
				m_Overlay = ViewportOverlay::Depth;

			ImGui::EndCombo();
		}

		ImRect comboBoxRect = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax() };
		drawList->AddRect(
			comboBoxRect.Min,
			comboBoxRect.Max,
			ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Border]), style.FrameRounding, 0, 1.5f);

		ImGui::PopID();


		// Guizmos
		float width = 30.0f * 3.0f;
		float offset = style.ItemSpacing.x + ImGui::GetItemRectSize().x;
		float buttonHeight = style.FramePadding.y * 2 + ImGui::GetFontSize();

		EditorLayer& editorLayer = EditorLayer::GetInstance();
		drawList->AddRectFilled(
			viewportImageRect.Min + ImVec2(offset, 0.0f), 
			viewportImageRect.Min + ImVec2(offset + width, buttonHeight),
			ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_FrameBg]), style.FrameRounding);

		drawList->AddRect(
			viewportImageRect.Min + ImVec2(offset, 0.0f),
			viewportImageRect.Min + ImVec2(offset + width, buttonHeight),
			ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Border]), style.FrameRounding, 0, 1.5f);

		ImGui::SameLine();

		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
			if (GuizmoButton("T", m_Guizmo == GuizmoMode::Translate))
				m_Guizmo = GuizmoMode::Translate;

			ImGui::SameLine();
			if (GuizmoButton("R", m_Guizmo == GuizmoMode::Rotate))
				m_Guizmo = GuizmoMode::Rotate;

			ImGui::SameLine();
			if (GuizmoButton("S", m_Guizmo == GuizmoMode::Scale))
				m_Guizmo = GuizmoMode::Scale;

			ImGui::PopStyleVar(); // Item spacing
		}

		// Scene View Settings

		ImGui::SameLine();
		
		{
			ImGui::PushID("SceneViewSettings");
			SceneViewSettings& settings = EditorLayer::GetInstance().GetSceneViewSettings();
			if (ImGui::BeginCombo("", "Settings"))
			{
				ImGui::MenuItem("Shadows", nullptr, &m_Viewport.ShadowMappingEnabled);
				ImGui::MenuItem("Post Processing", nullptr, &m_Viewport.PostProcessingEnabled);

				ImGui::Separator();

				ImGui::MenuItem("Show AABBs", nullptr, &settings.ShowAABBs);
				ImGui::MenuItem("Show Lights", nullptr, &settings.ShowLights);
				ImGui::MenuItem("Show Camera Frustums", nullptr, &settings.ShowCameraFrustum);
				ImGui::MenuItem("Show Grid", nullptr, &settings.ShowGrid);

				ImGui::EndCombo();
			}
			ImGui::PopID();
		}

		ImGui::PopItemWidth();

		ImGui::PopStyleVar(); // Window Padding

		ImGui::SetCursorPos(initialCursorPosition);
	}

	void SceneViewportWindow::RenderGrid()
	{
		if (!m_GridMaterial || !EditorLayer::GetInstance().GetSceneViewSettings().ShowGrid)
			return;

		float scale = m_Camera.GetZoom() * 3.0f;
		float cellScale = 5.0f + glm::floor(m_Camera.GetZoom() / 20.0f) * 5.0f;
		glm::vec3 gridColor = glm::vec3(0.5f);

		glm::vec3 cameraPosition = m_Camera.GetRotationOrigin();

		m_GridMaterial->WritePropertyValue(s_GridPropertyIndices.Offset, glm::vec3(cameraPosition.x, 0.0f, cameraPosition.z));
		m_GridMaterial->WritePropertyValue(s_GridPropertyIndices.Scale, scale);
		m_GridMaterial->WritePropertyValue(s_GridPropertyIndices.Thickness, 0.01f);
		m_GridMaterial->WritePropertyValue(s_GridPropertyIndices.CellScale, 1.0f / cellScale);
		m_GridMaterial->WritePropertyValue(s_GridPropertyIndices.Color, gridColor);
		m_GridMaterial->WritePropertyValue(s_GridPropertyIndices.FallOffThreshold, 0.8f);

		// TOOD: implement
	}

	void SceneViewportWindow::HandleAssetDragAndDrop(AssetHandle handle)
	{
		std::optional<Entity> entity = GetEntityUnderCursor();

		World& world = GetScene()->GetECSWorld();
		const AssetMetadata* metadata = AssetManager::GetAssetMetadata(handle);
		if (metadata != nullptr)
		{
			switch (metadata->Type)
			{
			case AssetType::Scene:
				EditorLayer::GetInstance().OpenScene(handle);
				break;
			case AssetType::Sprite:
			{
				if (!entity || !world.IsEntityAlive(*entity))
					break;

				SpriteComponent* sprite = world.TryGetEntityComponent<SpriteComponent>(*entity);
				if (sprite)
					sprite->Sprite = AssetManager::GetAsset<Sprite>(handle);

				break;
			}
			case AssetType::Material:
			{
				if (!entity || !world.IsEntityAlive(*entity))
					break;

				MeshComponent* meshComponent = world.TryGetEntityComponent<MeshComponent>(*entity);
				if (meshComponent)
					meshComponent->Material = handle;
				else
				{
					MaterialComponent* materialComponent = world.TryGetEntityComponent<MaterialComponent>(*entity);
					if (materialComponent)
						materialComponent->Material = handle;
				}

				break;
			}
			case AssetType::Prefab:
			{
				Ref<Prefab> prefab = AssetManager::GetAsset<Prefab>(handle);
				prefab->CreateInstance(GetScene()->GetECSWorld());
				break;
			}
			}
		}
	}

	std::optional<Entity> SceneViewportWindow::GetEntityUnderCursor() const
	{
		return {};
	}
}
