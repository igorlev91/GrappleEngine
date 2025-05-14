#include "EntitiesHierarchy.h"

#include "Grapple/Scene/Components.h"

#include "GrappleEditor/EditorLayer.h"
#include "GrappleEditor/UI/EditorGUI.h"
#include "GrappleEditor/ImGui/ImGuiLayer.h"

#include "GrappleEditor/Serialization/SerializationId.h"

#include <imgui.h>

namespace Grapple
{
	EntitiesHierarchy::EntitiesHierarchy(EntitiesHierarchyFeatures features)
		: m_World(nullptr), m_Features(features) {}

	EntitiesHierarchy::EntitiesHierarchy(World& world, EntitiesHierarchyFeatures features)
		: m_World(&world), m_Features(features) {}

	bool EntitiesHierarchy::OnRenderImGui(Entity& selectedEntity)
	{
		Grapple_CORE_ASSERT(m_World);

		bool result = false;
		const std::vector<EntityRecord>& records = m_World->Entities.GetEntityRecords();

		ImGui::BeginChild("Scene Entities");
		ImGuiListClipper clipper;
		clipper.Begin((int32_t)records.size());

		if (ImGui::BeginPopupContextWindow("Entity Hierarchy Context Menu"))
		{
			if (HAS_BIT(m_Features, EntitiesHierarchyFeatures::CreateEntity))
			{
				if (ImGui::BeginMenu("Create"))
				{
					if (ImGui::MenuItem("Entity"))
					{
						selectedEntity = m_World->CreateEntity<TransformComponent, SerializationId>();
						result = true;
					}

					if (ImGui::MenuItem("Perspective Camera"))
					{
						selectedEntity = m_World->CreateEntity(
							TransformComponent(),
							SerializationId(),
							CameraComponent(CameraComponent::ProjectionType::Perspective));
						result = true;
					}

					if (ImGui::MenuItem("Orthographic Camera"))
					{
						selectedEntity = m_World->CreateEntity(
							TransformComponent(),
							SerializationId(),
							CameraComponent(CameraComponent::ProjectionType::Orthographic));
						result = true;
					}

					if (ImGui::MenuItem("Directional Light"))
					{
						selectedEntity = m_World->CreateEntity(TransformComponent(), SerializationId(), DirectionalLight());
						result = true;
					}

					if (ImGui::MenuItem("Point Light"))
					{
						selectedEntity = m_World->CreateEntity(TransformComponent(), SerializationId(), PointLight());
						result = true;
					}

					if (ImGui::MenuItem("Environment"))
					{
						selectedEntity = m_World->CreateEntity(TransformComponent(), SerializationId(), Environment());
						result = true;
					}

					ImGui::EndMenu();
				}
			}

			ImGui::EndMenu();
		}

		std::optional<Entity> deletedEntity;
		while (clipper.Step())
		{
			for (int32_t i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			{
				if (i < 0 || i >= (int32_t)records.size())
					continue;

				Entity entity = records[i].Id;
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
					| ImGuiTreeNodeFlags_FramePadding
					| ImGuiTreeNodeFlags_Leaf
					| ImGuiTreeNodeFlags_SpanFullWidth;

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImGui::GetStyle().FramePadding / 2);

				bool selected = entity == selectedEntity;
				if (selected)
					flags |= ImGuiTreeNodeFlags_Selected;

				NameComponent* entityName = m_World->TryGetEntityComponent<NameComponent>(entity);

				bool opened = false;
				if (entityName)
					opened = ImGui::TreeNodeEx((void*)std::hash<Entity>()(entity), flags, entityName->Value.c_str());
				else
					opened = ImGui::TreeNodeEx((void*)std::hash<Entity>()(entity), flags, "Entity %d", entity.GetIndex());

				ImGui::PopStyleVar(1);

				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload(ENTITY_PAYLOAD_NAME, &entity, sizeof(Entity));
					ImGui::EndDragDropSource();
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
				{
					selectedEntity = entity;
					result = true;
				}

				if (opened)
					ImGui::TreePop();

				if (ImGui::BeginPopupContextItem())
				{
					if (HAS_BIT(m_Features, EntitiesHierarchyFeatures::DeleteEntity) && ImGui::MenuItem("Delete"))
						m_World->DeleteEntity(entity);

					if (HAS_BIT(m_Features, EntitiesHierarchyFeatures::DuplicateEntity) && ImGui::MenuItem("Duplicate"))
					{
						Entities& entities = m_World->Entities;
						ArchetypeId archetype = entities.GetEntityArchetype(entity);
						Entity duplicated = entities.CreateEntityFromArchetype(archetype, ComponentInitializationStrategy::DefaultConstructor);

						const ArchetypeRecord& record = m_World->GetArchetypes()[archetype];

						uint8_t* source = entities.GetEntityData(entity).value_or(nullptr);
						uint8_t* destination = entities.GetEntityData(duplicated).value_or(nullptr);

						Grapple_CORE_ASSERT(source && destination);

						for (size_t i = 0; i < record.Components.size(); i++)
						{
							if (record.Components[i] == COMPONENT_ID(SerializationId))
							{
								// NOTE: Skip SerializationId component, because each entity must have a unique serialization id.
								//
								//       Copying the id will result in problems when deserialing entity references,
								//       SceneSerializer might deserialize the wrong entity reference because there will
								//       be multiple entities with the same id
								//
								//       A unique id is generated by initializing components using a default constructor earlier
								continue;
							}

							uint8_t* componentSource = source + record.ComponentOffsets[i];
							uint8_t* componentDestination = destination + record.ComponentOffsets[i];

							Grapple_CORE_ASSERT(m_World->Components.IsComponentIdValid(record.Components[i]));
							const ComponentInfo& component = m_World->Components.GetComponentInfo(record.Components[i]);

							component.Initializer->Type.CopyConstructor(componentDestination, componentSource);
						}

						selectedEntity = duplicated;
						result = true;
					}

					ImGui::EndMenu();
				}
			}
		}

		clipper.End();
		ImGui::EndChild();

		return result;
	}
}
