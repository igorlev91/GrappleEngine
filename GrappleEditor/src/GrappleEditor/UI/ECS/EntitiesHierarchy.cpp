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
						m_World->CreateEntity<TransformComponent, SerializationId>();

					if (ImGui::MenuItem("Perspective Camera"))
					{
						m_World->CreateEntity(
							TransformComponent(),
							SerializationId(),
							CameraComponent(CameraComponent::ProjectionType::Perspective));
					}

					if (ImGui::MenuItem("Orthographic Camera"))
					{
						m_World->CreateEntity(
							TransformComponent(),
							SerializationId(),
							CameraComponent(CameraComponent::ProjectionType::Orthographic));
					}

					if (ImGui::MenuItem("Directional Light"))
					{
						m_World->CreateEntity(TransformComponent(), SerializationId(), DirectionalLight());
					}

					if (ImGui::MenuItem("Environment"))
					{
						m_World->CreateEntity(TransformComponent(), SerializationId(), Environment());
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
						Entity duplicated = entities.CreateEntityFromArchetype(archetype, ComponentInitializationStrategy::Zero);

						const ArchetypeRecord& record = m_World->GetArchetypes()[archetype];

						// TODO: use a copy constructor instead
						std::memcpy(entities.GetEntityData(duplicated).value(),
							entities.GetEntityData(entity).value(),
							entities.GetEntityStorage(archetype).GetEntitySize());

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
