#pragma once

#include "Grapple/Core/Assert.h"
#include "Grapple/Core/Core.h"

#include "GrappleECS/Entity.h"
#include "GrappleECS/Component.h"
#include "GrappleECS/Archetype.h"

#include <unordered_map>
#include <vector>
#include <optional>

namespace Grapple
{
	class EntityView;
	class EntityRegistryIterator;

	class Registry
	{
	public:
		Entity CreateEntity(ComponentSet& components);
		bool AddEntityComponent(Entity entity, ComponentId componentId, const void* componentData);
		bool RemoveEntityComponent(Entity entity, ComponentId componentId);

		EntityView View(ComponentSet components);
		const ComponentSet& GetEntityComponents(Entity entity);
		bool HasComponent(Entity entity, ComponentId component);

		std::optional<void*> GetEntityComponent(Entity entity, ComponentId component);

		ComponentId RegisterComponent(std::string_view name, size_t size);

		inline ArchetypeRecord& GetArchetypeRecord(size_t archetypeId) { return m_Archetypes[archetypeId]; }
		
		inline const ComponentInfo& GetComponentInfo(size_t index) const;
		inline const std::vector<ComponentInfo>& GetRegisteredComponents() const { return m_RegisteredComponents; }

		std::optional<size_t> GetArchetypeComponentIndex(ArchetypeId archetype, ComponentId component);

		EntityRegistryIterator begin();
		EntityRegistryIterator end();
	public:
		inline EntityRecord& operator[](size_t index);
		inline const EntityRecord& operator[](size_t index) const;
	private:
		ArchetypeId CreateArchetype();

		void RemoveEntityData(ArchetypeId archetype, size_t entityBufferIndex);
	private:
		std::vector<EntityRecord> m_EntityRecords;
		std::unordered_map<Entity, size_t> m_EntityToRecord;

		std::vector<ArchetypeRecord> m_Archetypes;
		std::unordered_map<ComponentSet, size_t> m_ComponentSetToArchetype;

		std::unordered_map<ComponentId, std::unordered_map<ArchetypeId, size_t>> m_ComponentToArchetype;

		std::vector<ComponentInfo> m_RegisteredComponents;

		friend class EntityRegistryIterator;
	};
}