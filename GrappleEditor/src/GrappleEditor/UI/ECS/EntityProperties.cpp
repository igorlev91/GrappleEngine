#include "EntityProperties.h"

#include "Grapple/AssetManager/AssetManager.h"
#include "Grapple/Renderer/Renderer.h"

#include "GrappleEditor/UI/EditorGUI.h"

#include <imgui.h>

namespace Grapple
{
	EntityProperties::EntityProperties(World& world)
		: m_World(world) {}

	void EntityProperties::OnRenderImGui(Entity entity)
	{
		if (!m_World.IsEntityAlive(entity))
			return;

		ImGui::BeginDisabled();
		ImGui::Text("Index %d Generation %d", entity.GetIndex(), entity.GetGeneration());
		ImGui::EndDisabled();

		std::string newNameString = "";
		std::string* nameString = &newNameString;

		NameComponent* name = m_World.TryGetEntityComponent<NameComponent>(entity);
		if (name)
			nameString = &name->Value;

		if (EditorGUI::TextField("Name", *nameString))
		{
			if (!nameString->empty() && !name)
			{
				m_World.AddEntityComponent<NameComponent>(entity, NameComponent());
				m_World.GetEntityComponent<NameComponent>(entity).Value = *nameString;
			}

			if (nameString->empty() && name)
				m_World.RemoveEntityComponent<NameComponent>(entity);
		}

		RenderAddComponentMenu(entity);

		if (ImGui::BeginChild("Components"))
		{
			std::optional<ComponentId> removedComponent;
			for (ComponentId component : m_World.GetEntityComponents(entity))
			{
				const ComponentInfo& componentInfo = m_World.Components.GetComponentInfo(component);
				if (component == COMPONENT_ID(CameraComponent))
					RenderCameraComponent(m_World.GetEntityComponent<CameraComponent>(entity));
				else if (component == COMPONENT_ID(SpriteComponent))
					RenderSpriteComponent(m_World.GetEntityComponent<SpriteComponent>(entity));
				else if (component == COMPONENT_ID(MeshComponent))
					RenderMeshComponent(m_World.GetEntityComponent<MeshComponent>(entity));
				else if (component == COMPONENT_ID(Environment))
					RenderEnvironmentComponent(m_World.GetEntityComponent<Environment>(entity));
				else
				{
					std::optional<void*> componentData = m_World.Entities.GetEntityComponent(entity, component);
					if (componentData.has_value() && componentInfo.Initializer)
					{
						EditorGUI::ObjectField(componentInfo.Initializer->Type.SerializationDescriptor, componentData.value(), &m_World);
					}
				}

				if (ImGui::BeginPopupContextItem(componentInfo.Name.c_str()))
				{
					if (ImGui::MenuItem("Remove"))
						removedComponent = component;

					ImGui::End();
				}
			}

			if (removedComponent.has_value())
				m_World.Entities.RemoveEntityComponent(entity, removedComponent.value());
		}

		ImGui::EndChild();
	}


	void EntityProperties::RenderCameraComponent(CameraComponent& cameraComponent)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_FramePadding;
		if (ImGui::TreeNodeEx((void*)std::hash<ComponentId>()(COMPONENT_ID(CameraComponent)), flags, "Camera"))
		{
			if (EditorGUI::BeginPropertyGrid())
			{
				EditorGUI::BeginToggleGroupProperty("Projection", 2);
				if (EditorGUI::ToggleGroupItem("Orthographic", cameraComponent.Projection == CameraComponent::ProjectionType::Orthographic))
					cameraComponent.Projection = CameraComponent::ProjectionType::Orthographic;
				if (EditorGUI::ToggleGroupItem("Perspective", cameraComponent.Projection == CameraComponent::ProjectionType::Perspective))
					cameraComponent.Projection = CameraComponent::ProjectionType::Perspective;

				EditorGUI::EndToggleGroup();

				if (cameraComponent.Projection == CameraComponent::ProjectionType::Orthographic)
					EditorGUI::FloatPropertyField("Size", cameraComponent.Size);
				else
					EditorGUI::FloatPropertyField("FOV", cameraComponent.FOV);

				EditorGUI::FloatPropertyField("Near", cameraComponent.Near);
				EditorGUI::FloatPropertyField("Far", cameraComponent.Far);

				EditorGUI::EndPropertyGrid();
			}

			ImGui::TreePop();
		}
	}

	void EntityProperties::RenderSpriteComponent(SpriteComponent& sprite)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_FramePadding;
		if (ImGui::TreeNodeEx((void*)std::hash<ComponentId>()(COMPONENT_ID(SpriteComponent)), flags, "Sprite"))
		{
			if (EditorGUI::BeginPropertyGrid())
			{
				EditorGUI::ColorPropertyField("Color", sprite.Color);

				AssetHandle spriteHandle = sprite.Sprite == nullptr ? NULL_ASSET_HANDLE : sprite.Sprite->Handle;
				if (EditorGUI::AssetField("Sprite", spriteHandle, &Sprite::_Asset))
					sprite.Sprite = AssetManager::GetAsset<Sprite>(spriteHandle);

				EditorGUI::Vector2PropertyField("Tiling", sprite.Tilling);

				EditorGUI::PropertyName("Flip");
				{
					bool flipX = HAS_BIT(sprite.Flags, SpriteRenderFlags::FlipX);
					bool flipY = HAS_BIT(sprite.Flags, SpriteRenderFlags::FlipY);

					ImGui::Checkbox("X", &flipX);
					ImGui::SameLine();
					ImGui::Checkbox("Y", &flipY);

					sprite.Flags = sprite.Flags & ~(SpriteRenderFlags::FlipX | SpriteRenderFlags::FlipY);

					if (flipX)
						sprite.Flags |= SpriteRenderFlags::FlipX;
					if (flipY)
						sprite.Flags |= SpriteRenderFlags::FlipY;
				}

				EditorGUI::EndPropertyGrid();
			}

			ImGui::TreePop();
		}
	}

	void EntityProperties::RenderMeshComponent(MeshComponent& mesh)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_FramePadding;
		if (ImGui::TreeNodeEx((void*)std::hash<ComponentId>()(COMPONENT_ID(MeshComponent)), flags, "Mesh"))
		{
			if (EditorGUI::BeginPropertyGrid())
			{
				AssetHandle meshHandle = NULL_ASSET_HANDLE;
				if (mesh.Mesh)
					meshHandle = mesh.Mesh->Handle;
				if (EditorGUI::AssetField("Mesh", meshHandle, &Mesh::_Asset))
					mesh.Mesh = AssetManager::GetAsset<Mesh>(meshHandle);

				EditorGUI::AssetField("Material", mesh.Material, nullptr);

				const char* propertyName = "Don't cast shadows";
				EditorGUI::PropertyName(propertyName);
				{
					bool value = HAS_BIT(mesh.Flags, MeshRenderFlags::DontCastShadows);

					ImGui::PushID(propertyName);
					ImGui::Checkbox("", &value);
					ImGui::PopID();

					if (value)
						mesh.Flags |= MeshRenderFlags::DontCastShadows;
					else
						mesh.Flags &= ~MeshRenderFlags::DontCastShadows;
				}

				EditorGUI::EndPropertyGrid();
			}

			ImGui::TreePop();
		}
	}

	void EntityProperties::RenderEnvironmentComponent(Environment& environment)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_FramePadding;
		if (ImGui::TreeNodeEx((void*)std::hash<ComponentId>()(COMPONENT_ID(CameraComponent)), flags, "Environment"))
		{
			if (EditorGUI::BeginPropertyGrid())
			{
				EditorGUI::ColorPropertyField("Environment Color", environment.EnvironmentColor);
				EditorGUI::FloatPropertyField("Environment Color Intensity", environment.EnvironmentColorIntensity);

				EditorGUI::BoolPropertyField("Shadows Enabled", environment.ShadowSettings.Enabled);

				ImGui::BeginDisabled(!environment.ShadowSettings.Enabled);

				EditorGUI::PropertyName("Shadow Quality");

				const char* qualityPreviews[] = { "Low", "Medium", "High" };
				ShadowQuality qualityValues[] =
				{
					ShadowQuality::Low,
					ShadowQuality::Medium,
					ShadowQuality::High,
				};

				uint32_t qualityIndex = 0;
				for (uint32_t i = 0; i < 3; i++)
				{
					if (qualityValues[i] == environment.ShadowSettings.Quality)
						qualityIndex = i;
				}

				if (ImGui::BeginCombo("Quality", qualityPreviews[qualityIndex]))
				{
					for (size_t i = 0; i < 3; i++)
					{
						if (ImGui::MenuItem(qualityPreviews[i]))
							environment.ShadowSettings.Quality = qualityValues[i];
					}

					ImGui::EndCombo();
				}

				EditorGUI::FloatPropertyField("Shadow Bias", environment.ShadowSettings.Bias);
				EditorGUI::FloatPropertyField("Shadow Normal Bias", environment.ShadowSettings.NormalBias);
				EditorGUI::IntPropertyField("Shadow Cascades", environment.ShadowSettings.Cascades);
				EditorGUI::FloatPropertyField("Softness", environment.ShadowSettings.Softness);
				EditorGUI::FloatPropertyField("Light Size", environment.ShadowSettings.LightSize);
				EditorGUI::FloatPropertyField("Fade Distance", environment.ShadowSettings.FadeDistance);

				EditorGUI::FloatPropertyField("Shadow Cascade 0", environment.ShadowSettings.CascadeSplits[0]);
				EditorGUI::FloatPropertyField("Shadow Cascade 1", environment.ShadowSettings.CascadeSplits[1]);
				EditorGUI::FloatPropertyField("Shadow Cascade 2", environment.ShadowSettings.CascadeSplits[2]);
				EditorGUI::FloatPropertyField("Shadow Cascade 3", environment.ShadowSettings.CascadeSplits[3]);

				ImGui::EndDisabled();

				EditorGUI::EndPropertyGrid();
			}

			ImGui::TreePop();
		}
	}

	void EntityProperties::RenderAddComponentMenu(Entity entity)
	{
		if (ImGui::BeginMenu("Add component"))
		{
			const std::vector<ComponentInfo>& components = m_World.Components.GetRegisteredComponents();

			for (const auto& info : components)
			{
				if (ImGui::MenuItem(info.Name.c_str()))
					m_World.Entities.AddEntityComponent(entity, info.Id, nullptr);
			}

			ImGui::End();
		}
	}
}
