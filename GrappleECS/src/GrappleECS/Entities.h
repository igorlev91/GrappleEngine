#pragma once

#include "GrappleCore/Assert.h"
#include "GrappleCore/Core.h"

#include "GrappleECS/Entity/Entity.h"
#include "GrappleECS/Entity/Component.h"
#include "GrappleECS/Entity/ComponentInitializer.h"
#include "GrappleECS/Entity/Archetype.h"
#include "GrappleECS/Entity/Archetypes.h"
#include "GrappleECS/Entity/EntityIndex.h"

#include "GrappleECS/Query/QueryCache.h"

#include <unordered_map>
#include <vector>
#include <optional>

namespace Grapple
{
	class GrappleECS_API Query;

	class EntityView;
	class EntitiesIterator;

	enum class ComponentInitializationStrategy : uint8_t
	{
		Zero,
		DefaultConstructor,
	};

	class GrappleECS_API Entities
	{
	public:
		Entities(QueryCache& queries, Archetypes& archetypes);
		Entities(const Entities&) = delete;

		Entities& operator=(const Entities&) = delete;

		~Entities();
	public:
		// Entity operations

		Entity CreateEntity(const ComponentSet& componentSet, 
			ComponentInitializationStrategy initStrategy = ComponentInitializationStrategy::DefaultConstructor);

		// Components must be sorted by [ComponentId]
		Entity CreateEntity(const std::pair<ComponentId, const void*>* components, size_t count);

		Entity CreateEntityFromArchetype(ArchetypeId archetype,
			ComponentInitializationStrategy initStrategy = ComponentInitializationStrategy::DefaultConstructor);

		void DeleteEntity(Entity entity);

		bool AddEntityComponent(Entity entity, 
			ComponentId componentId, 
			const void* componentData, 
			ComponentInitializationStrategy initStrategy = ComponentInitializationStrategy::DefaultConstructor);

		bool RemoveEntityComponent(Entity entity, ComponentId componentId);
		bool IsEntityAlive(Entity entity) const;

		ArchetypeId GetEntityArchetype(Entity entity);

		const std::vector<EntityRecord>& GetEntityRecords() const;

		std::optional<Entity> FindEntityByIndex(uint32_t entityIndex);
		std::optional<Entity> FindEntityByRegistryIndex(uint32_t registryIndex);

		std::optional<uint8_t*> GetEntityData(Entity entity);
		std::optional<const uint8_t*> GetEntityData(Entity entity) const;
		std::optional<size_t> GetEntityDataSize(Entity entity) const;

		EntityStorage& GetEntityStorage(ArchetypeId archetype);
		const EntityStorage& GetEntityStorage(ArchetypeId archetype) const;

		// Component operations

		void RegisterComponents();

		ComponentId RegisterComponent(std::string_view name, size_t size, const std::function<void(void*)>& deleter);
		std::optional<ComponentId> FindComponnet(std::string_view name);
		bool IsComponentIdValid(ComponentId id) const;

		std::optional<void*> GetEntityComponent(Entity entity, ComponentId component);
		std::optional<const void*> GetEntityComponent(Entity entity, ComponentId component) const;

		const std::vector<ComponentId>& GetEntityComponents(Entity entity);
		bool HasComponent(Entity entity, ComponentId component) const;

		const ComponentInfo& GetComponentInfo(ComponentId id) const;
		const std::vector<ComponentInfo>& GetRegisteredComponents() const;

		std::optional<void*> GetSingletonComponent(ComponentId id) const;
		std::optional<Entity> GetSingletonEntity(const Query& query) const;

		// Archetypes

		inline const Archetypes& GetArchetypes() const { return m_Archetypes; }
		
		// Iterator

		EntitiesIterator begin();
		EntitiesIterator end();
	public:
		EntityRecord& operator[](size_t index);
		const EntityRecord& operator[](size_t index) const;
	private:
		struct EntityCreationResult
		{
			Entity Id;
			ArchetypeId Archetype;
			uint8_t* Data;
		};

		void CreateEntity(const ComponentSet& components, EntityCreationResult& result);
		void InitializeEntity(const EntityCreationResult& entityResult, ComponentInitializationStrategy initStrategy);

		ComponentId RegisterComponent(ComponentInitializer& initializer);
		ArchetypeId CreateArchetype();

		void RemoveEntityData(ArchetypeId archetype, size_t entityBufferIndex);

		std::unordered_map<Entity, size_t>::iterator FindEntity(Entity entity);
		std::unordered_map<Entity, size_t>::const_iterator FindEntity(Entity entity) const;
	private:
		std::vector<ComponentId> m_TemporaryComponentSet;

		Archetypes& m_Archetypes;
		QueryCache& m_Queries;

		std::vector<EntityStorage> m_EntityStorages;

		std::vector<EntityRecord> m_EntityRecords;
		std::unordered_map<Entity, size_t> m_EntityToRecord;

		std::unordered_map<std::string, uint32_t> m_ComponentNameToIndex;
		std::unordered_map<ComponentId, uint32_t> m_ComponentIdToIndex;
		std::vector<ComponentInfo> m_RegisteredComponents;

		EntityIndex m_EntityIndex;

		friend class EntitiesIterator;
		friend class QueryCache;
	};
}