#include "PropertiesWindow.h"

#include "Grapple/Scene/Scene.h"
#include "Grapple/Scripting/ScriptingEngine.h"

#include "GrappleEditor/UI/EditorGUI.h"
#include "GrappleEditor/EditorLayer.h"

#include <imgui.h>

namespace Grapple
{
	void PropertiesWindow::OnImGuiRender()
	{
		World& world = Scene::GetActive()->GetECSWorld();
		Entity selectedEntity = EditorLayer::GetInstance().GetSelectedEntity();

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();

		ImGui::Begin("Properties");

		if (world.IsEntityAlive(selectedEntity))
		{
			ImGui::BeginDisabled();
			ImGui::Text("Index %d Generation %d", selectedEntity.GetIndex(), selectedEntity.GetGeneration());
			ImGui::EndDisabled();
			
			RenderAddComponentMenu(selectedEntity);

			std::optional<ComponentId> removedComponent;
			for (ComponentId component : world.GetEntityComponents(selectedEntity))
			{
				const ComponentInfo& componentInfo = world.GetRegistry().GetComponentInfo(component);

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_FramePadding;
				if (ImGui::TreeNodeEx((void*)std::hash<ComponentId>()(component), flags, "%s", componentInfo.Name.c_str()))
				{
					if (component == CameraComponent::Id)
						RenderCameraComponent(world.GetEntityComponent<CameraComponent>(selectedEntity));
					else if (component == TransformComponent::Id)
						RenderTransformComponent(world.GetEntityComponent<TransformComponent>(selectedEntity));
					else if (component == SpriteComponent::Id)
						RenderSpriteComponent(world.GetEntityComponent<SpriteComponent>(selectedEntity));
					else
					{
						std::optional<void*> componentData = world.GetRegistry().GetEntityComponent(selectedEntity, component);
						if (componentData.has_value())
						{
							std::optional<const Internal::ScriptingType*> type = ScriptingEngine::FindComponentType(component);
							if (type.has_value())
								EditorGUI::TypeEditor(*(type.value()), (uint8_t*) componentData.value());
						}
					}

					ImGui::TreePop();
				}

				if (ImGui::BeginPopupContextItem(componentInfo.Name.c_str()))
				{
					if (ImGui::MenuItem("Remove"))
						removedComponent = component;

					ImGui::End();
				}
			}

			if (removedComponent.has_value())
				world.GetRegistry().RemoveEntityComponent(selectedEntity, removedComponent.value());
		}

		ImGui::End();
	}

	void PropertiesWindow::RenderAddComponentMenu(Entity entity)
	{
		if (ImGui::BeginMenu("Add component"))
		{
			World& world = Scene::GetActive()->GetECSWorld();
			const std::vector<ComponentInfo>& components = world.GetRegistry().GetRegisteredComponents();

			for (const auto& info : components)
			{
				if (ImGui::MenuItem(info.Name.c_str()))
					world.GetRegistry().AddEntityComponent(entity, info.Id, nullptr);
			}

			ImGui::End();
		}
	}

	void PropertiesWindow::RenderCameraComponent(CameraComponent& cameraComponent)
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
	}

	void PropertiesWindow::RenderTransformComponent(TransformComponent& transform)
	{
		if (EditorGUI::BeginPropertyGrid())
		{
			EditorGUI::Vector3PropertyField("Position", transform.Position);
			EditorGUI::Vector3PropertyField("Rotation", transform.Rotation);
			EditorGUI::Vector3PropertyField("Scale", transform.Scale);

			EditorGUI::EndPropertyGrid();
		}
	}

	void PropertiesWindow::RenderSpriteComponent(SpriteComponent& sprite)
	{
		if (EditorGUI::BeginPropertyGrid())
		{
			EditorGUI::ColorPropertyField("Color", sprite.Color);
			EditorGUI::AssetField("Texture", sprite.Texture);
			EditorGUI::Vector2PropertyField("Tiling", sprite.TextureTiling);

			EditorGUI::EndPropertyGrid();
		}
	}
}