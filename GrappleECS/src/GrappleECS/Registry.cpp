#include "Registry.h"

#include "GrappleECS/Query/EntityView.h"
#include "GrappleECS/Query/EntityRegistryIterator.h"

#include "GrappleECS/Query/Query.h"

#include "GrappleECS/Entity/ComponentInitializer.h"

#include "GrappleECS/EntityStorage/EntityChunksPool.h"

#include <algorithm>

namespace Grapple
{
	Registry::Registry(QueryCache& queries, Archetypes& archetypes)
		: m_Queries(queries), m_Archetypes(archetypes)
	{
		EntityChunksPool::Initialize(16);
	}

	Registry::~Registry()
	{
		for (const ArchetypeRecord& archetype : m_Archetypes.Records)
		{
			EntityStorage& storage = m_EntityStorages[archetype.Id];
			for (size_t entityIndex = 0; entityIndex < storage.GetEntitiesCount(); entityIndex++)
			{
				uint8_t* entityData = storage.GetEntityData(entityIndex);
				for (size_t i = 0; i < archetype.Components.size(); i++)
				{
					GetComponentInfo(archetype.Components[i]).Deleter((void*)(entityData + archetype.ComponentOffsets[i]));
				}
			}
		}
	}

	Entity Registry::CreateEntity(const ComponentSet& componentSet, ComponentInitializationStrategy initStrategy)
	{
		Grapple_CORE_ASSERT(componentSet.GetCount() > 0);

		if (m_TemporaryComponentSet.size() < componentSet.GetCount())
			m_TemporaryComponentSet.resize(componentSet.GetCount());

		std::memcpy(m_TemporaryComponentSet.data(), componentSet.GetIds(), sizeof(ComponentId) * componentSet.GetCount());

		std::sort(m_TemporaryComponentSet.data(), m_TemporaryComponentSet.data() + componentSet.GetCount());

		ComponentSet components = ComponentSet(m_TemporaryComponentSet.data(), componentSet.GetCount());

		size_t registryIndex = m_EntityRecords.size();
		EntityRecord& record = m_EntityRecords.emplace_back();

		record.RegistryIndex = (uint32_t)registryIndex;
		record.Id = m_EntityIndex.CreateId();

		auto it = m_Archetypes.ComponentSetToArchetype.find(components);
		if (it != m_Archetypes.ComponentSetToArchetype.end())
			record.Archetype = it->second;
		else
		{
			ArchetypeId newArchetypeId = CreateArchetype();
			ArchetypeRecord& newArchetype = m_Archetypes.Records[newArchetypeId];
			record.Archetype = newArchetype.Id;

			newArchetype.ComponentOffsets.resize(components.GetCount());
			newArchetype.Components.resize(components.GetCount());

			std::memcpy(newArchetype.Components.data(), components.GetIds(), components.GetCount() * sizeof(size_t));

			size_t entitySize = 0;
			for (size_t i = 0; i < components.GetCount(); i++)
			{
				newArchetype.ComponentOffsets[i] = entitySize;
				entitySize += GetComponentInfo(components[i]).Size;
			}

			GetEntityStorage(newArchetypeId).SetEntitySize(entitySize);

			m_Archetypes.ComponentSetToArchetype.emplace(ComponentSet(newArchetype.Components), newArchetype.Id);

			for (size_t i = 0; i < newArchetype.Components.size(); i++)
				m_Archetypes.ComponentToArchetype[newArchetype.Components[i]].emplace(newArchetypeId, i);

			m_Queries.OnArchetypeCreated(newArchetypeId);
		}

		ArchetypeRecord& archetypeRecord = m_Archetypes.Records[record.Archetype];
		EntityStorage& storage = GetEntityStorage(record.Archetype);
		record.BufferIndex = storage.AddEntity(record.RegistryIndex);

		m_EntityToRecord.emplace(record.Id, record.RegistryIndex);
		
		uint8_t* entityData = storage.GetEntityData(record.BufferIndex);

		switch (initStrategy)
		{
			case ComponentInitializationStrategy::Zero:
			{
				// Intialize entity data to 0
				std::memset(entityData, 0, storage.GetEntitySize());
				break;
			}
			case ComponentInitializationStrategy::DefaultConstructor:
			{
				for (size_t i = 0; i < archetypeRecord.Components.size(); i++)
				{
					const ComponentInfo& info = GetComponentInfo(archetypeRecord.Components[i]);
					uint8_t* componentData = entityData + archetypeRecord.ComponentOffsets[i];

					if (info.Initializer)
						info.Initializer->Type.DefaultConstructor(componentData);
					else
						std::memset(componentData, 0, info.Size);
				}

				break;
			}
		}

		return record.Id;
	}

	Entity Registry::CreateEntityFromArchetype(ArchetypeId archetype, ComponentInitializationStrategy initStrategy)
	{
		Grapple_CORE_ASSERT((size_t)archetype < m_Archetypes.Records.size());

		size_t registryIndex = m_EntityRecords.size();
		EntityRecord& record = m_EntityRecords.emplace_back();

		record.RegistryIndex = (uint32_t)registryIndex;
		record.Id = m_EntityIndex.CreateId();
		record.Archetype = archetype;

		const ArchetypeRecord& archetypeRecord = m_Archetypes[archetype];
		EntityStorage& storage = GetEntityStorage(archetype);

		record.BufferIndex = storage.AddEntity(record.RegistryIndex);

		uint8_t* entityData = storage.GetEntityData(record.BufferIndex);
		switch (initStrategy)
		{
		case ComponentInitializationStrategy::Zero:
		{
			// Intialize entity data to 0
			std::memset(entityData, 0, storage.GetEntitySize());
			break;
		}
		case ComponentInitializationStrategy::DefaultConstructor:
		{
			for (size_t i = 0; i < archetypeRecord.Components.size(); i++)
			{
				const ComponentInfo& info = GetComponentInfo(archetypeRecord.Components[i]);
				uint8_t* componentData = entityData + archetypeRecord.ComponentOffsets[i];

				if (info.Initializer)
					info.Initializer->Type.DefaultConstructor(componentData);
				else
					std::memset(componentData, 0, info.Size);
			}

			break;
		}
		}

		m_EntityToRecord.emplace(record.Id, record.RegistryIndex);
		return record.Id;
	}

	void Registry::DeleteEntity(Entity entity)
	{
		auto recordIterator = FindEntity(entity);
		if (recordIterator == m_EntityToRecord.end())
			return;

		EntityRecord& record = m_EntityRecords[recordIterator->second];
		EntityRecord& lastEntityRecord = m_EntityRecords.back();

		EntityStorage& storage = GetEntityStorage(record.Archetype);

		uint32_t lastEntityInBuffer = storage.GetEntityIndices().back();
		if (lastEntityInBuffer != record.RegistryIndex)
			m_EntityRecords[lastEntityInBuffer].BufferIndex = record.BufferIndex;

		storage.RemoveEntityData(record.BufferIndex);

		m_EntityIndex.AddDeletedId(record.Id);
		m_EntityToRecord.erase(record.Id);

		lastEntityRecord.RegistryIndex = record.RegistryIndex;
		record = lastEntityRecord;

		if (lastEntityRecord.Id != entity)
		{
			GetEntityStorage(record.Archetype).UpdateEntityRegistryIndex(record.BufferIndex, record.RegistryIndex);
			m_EntityToRecord[record.Id] = record.RegistryIndex;
		}

		m_EntityRecords.erase(m_EntityRecords.end() - 1);
	}

	bool Registry::AddEntityComponent(Entity entity, ComponentId componentId, const void* componentData, ComponentInitializationStrategy initStrategy)
	{
		Grapple_CORE_ASSERT(IsComponentIdValid(componentId), "Invalid component id");

		const ComponentInfo& componentInfo = GetComponentInfo(componentId);

		auto recordIterator = FindEntity(entity);
		if (recordIterator == m_EntityToRecord.end())
			return {};

		EntityRecord& entityRecord = m_EntityRecords[recordIterator->second];
		ArchetypeRecord& archetype = m_Archetypes.Records[entityRecord.Archetype];

		// Can only have one instance of a component
		if (m_Archetypes.GetArchetypeComponentIndex(entityRecord.Archetype, componentId).has_value())
			return false;

		size_t oldComponentCount = archetype.Components.size();
		ArchetypeId newArchetypeId = INVALID_ARCHETYPE_ID;

		size_t insertedComponentIndex = SIZE_MAX;

		auto edgeIterator = archetype.Edges.find(componentId);
		if (edgeIterator != archetype.Edges.end())
		{
			newArchetypeId = edgeIterator->second.Add;
			
			std::optional<size_t> index = m_Archetypes.GetArchetypeComponentIndex(newArchetypeId, componentId);
			if (index.has_value())
				insertedComponentIndex = index.value();
			else
			{
				Grapple_CORE_ASSERT(false, "Archetype doesn't have a component because archetype graph has invalid edge connection");
				return false;
			}
		}
		else
		{
			std::vector<ComponentId> newComponents(oldComponentCount + 1);

			std::memcpy(newComponents.data(), archetype.Components.data(), oldComponentCount * sizeof(componentId));
			newComponents[oldComponentCount] = componentId;

			insertedComponentIndex = oldComponentCount;
			for (size_t i = insertedComponentIndex; i > 0; i--)
			{
				if (newComponents[i - 1] > newComponents[i])
				{
					std::swap(newComponents[i - 1], newComponents[i]);
					insertedComponentIndex = i - 1;
					break;
				}
			}

			bool shouldNotifyQueryCache = false;
			auto it = m_Archetypes.ComponentSetToArchetype.find(ComponentSet(newComponents));

			if (it != m_Archetypes.ComponentSetToArchetype.end())
			{
				newArchetypeId = it->second;
			}
			else
			{
				newArchetypeId = CreateArchetype();
				EntityStorage& storage = GetEntityStorage(newArchetypeId);

				ArchetypeRecord& newArchetype = m_Archetypes.Records[newArchetypeId];
				newArchetype.Components = std::move(newComponents);
				newArchetype.ComponentOffsets.resize(newArchetype.Components.size());

				size_t entitySize = 0;
				for (size_t i = 0; i < newArchetype.Components.size(); i++)
				{
					newArchetype.ComponentOffsets[i] = entitySize;
					entitySize += GetComponentInfo(newArchetype.Components[i]).Size;
				}

				storage.SetEntitySize(entitySize);
				m_Archetypes.ComponentSetToArchetype.emplace(ComponentSet(newArchetype.Components), newArchetypeId);

				shouldNotifyQueryCache = true;
			}

			m_Archetypes.Records[entityRecord.Archetype].Edges.emplace(componentId, ArchetypeEdge{newArchetypeId, INVALID_ARCHETYPE_ID});
			m_Archetypes.Records[newArchetypeId].Edges.emplace(componentId, ArchetypeEdge{INVALID_ARCHETYPE_ID, entityRecord.Archetype});

			ArchetypeRecord& newArchetype = m_Archetypes.Records[newArchetypeId];
			for (size_t i = 0; i < newArchetype.Components.size(); i++)
				m_Archetypes.ComponentToArchetype[newArchetype.Components[i]].emplace(newArchetypeId, i);

			if (shouldNotifyQueryCache)
				m_Queries.OnArchetypeCreated(newArchetypeId);
		}

		Grapple_CORE_ASSERT(insertedComponentIndex != SIZE_MAX);

		ArchetypeRecord& oldArchetype = m_Archetypes.Records[entityRecord.Archetype];
		ArchetypeRecord& newArchetype = m_Archetypes.Records[newArchetypeId];

		EntityStorage& oldStorage = GetEntityStorage(oldArchetype.Id);
		EntityStorage& newStorage = GetEntityStorage(newArchetypeId);

		size_t newEntityIndex = newStorage.AddEntity(entityRecord.RegistryIndex);
		uint8_t* newEntityData = newStorage.GetEntityData(newEntityIndex);
		const uint8_t* oldEntityData = oldStorage.GetEntityData(entityRecord.BufferIndex);

		size_t sizeBefore = oldStorage.GetEntitySize();
		if (insertedComponentIndex < oldArchetype.Components.size())
			sizeBefore = oldArchetype.ComponentOffsets[insertedComponentIndex];

		size_t sizeAfter = oldStorage.GetEntitySize() - sizeBefore;

		std::memcpy(newEntityData, oldEntityData, sizeBefore);
		std::memcpy(newEntityData + sizeBefore + componentInfo.Size, oldEntityData + sizeBefore, sizeAfter);

		if (componentData == nullptr)
		{
			uint8_t* componentData = newEntityData + sizeBefore;
			if (initStrategy == ComponentInitializationStrategy::Zero || !componentInfo.Initializer)
				std::memcpy(componentData, 0, componentInfo.Size);
			else
				componentInfo.Initializer->Type.DefaultConstructor(componentData);
		}
		else
			std::memcpy(newEntityData + sizeBefore, componentData, componentInfo.Size);

		RemoveEntityData(entityRecord.Archetype, entityRecord.BufferIndex);

		entityRecord.Archetype = newArchetypeId;
		entityRecord.BufferIndex = newEntityIndex;

		return true;
	}

	bool Registry::RemoveEntityComponent(Entity entity, ComponentId componentId)
	{
		Grapple_CORE_ASSERT(IsComponentIdValid(componentId), "Invalid component id");

		const ComponentInfo& componentInfo = GetComponentInfo(componentId);

		auto recordIterator = FindEntity(entity);
		if (recordIterator == m_EntityToRecord.end())
			return false;

		EntityRecord& entityRecord = m_EntityRecords[recordIterator->second];
		ArchetypeRecord& archetype = m_Archetypes.Records[entityRecord.Archetype];

		size_t removedComponentIndex = SIZE_MAX;
		ArchetypeId newArchetypeId = INVALID_ARCHETYPE_ID;
		
		{
			std::optional<size_t> componentIndex = m_Archetypes.GetArchetypeComponentIndex(entityRecord.Archetype, componentId);
			if (componentIndex.has_value())
				removedComponentIndex = componentIndex.value();
			else
				return false;
		}

		auto edgeIterator = archetype.Edges.find(componentId);
		if (edgeIterator != archetype.Edges.end())
		{
			newArchetypeId = edgeIterator->second.Remove;
		}
		else
		{
			size_t oldComponentCount = archetype.Components.size();
			std::vector<ComponentId> newComponents(oldComponentCount - 1);

			for (size_t insertIndex = 0, i = 0; i < oldComponentCount; i++)
			{
				if (archetype.Components[i] == componentId)
					continue;
				else
				{
					newComponents[insertIndex] = archetype.Components[i];
					insertIndex++;
				}
			}

			auto it = m_Archetypes.ComponentSetToArchetype.find(ComponentSet(newComponents));
			if (it != m_Archetypes.ComponentSetToArchetype.end())
			{
				newArchetypeId = it->second;
			}
			else
			{
				newArchetypeId = CreateArchetype();
				EntityStorage& storage = GetEntityStorage(newArchetypeId);

				ArchetypeRecord& newArchetype = m_Archetypes.Records[newArchetypeId];
				newArchetype.Components = std::move(newComponents);
				newArchetype.ComponentOffsets.resize(newArchetype.Components.size());

				size_t entitySize = 0;
				for (size_t i = 0; i < newArchetype.Components.size(); i++)
				{
					newArchetype.ComponentOffsets[i] = entitySize;
					entitySize += GetComponentInfo(newArchetype.Components[i]).Size;
				}

				storage.SetEntitySize(entitySize);
				m_Archetypes.ComponentSetToArchetype.emplace(ComponentSet(newArchetype.Components), newArchetypeId);
			}

			m_Archetypes.Records[entityRecord.Archetype].Edges.emplace(componentId, ArchetypeEdge{ INVALID_ARCHETYPE_ID, newArchetypeId});
			m_Archetypes.Records[newArchetypeId].Edges.emplace(componentId, ArchetypeEdge{ entityRecord.Archetype, INVALID_ARCHETYPE_ID });

			ArchetypeRecord& newArchetype = m_Archetypes.Records[newArchetypeId];
			for (size_t i = 0; i < newArchetype.Components.size(); i++)
				m_Archetypes.ComponentToArchetype[newArchetype.Components[i]].emplace(newArchetypeId, i);
		}

		ArchetypeRecord& oldArchetype = m_Archetypes.Records[entityRecord.Archetype];
		ArchetypeRecord& newArchetype = m_Archetypes.Records[newArchetypeId];

		EntityStorage& oldStorage = GetEntityStorage(oldArchetype.Id);
		EntityStorage& newStorage = GetEntityStorage(newArchetypeId);

		size_t sizeBefore = oldArchetype.ComponentOffsets[removedComponentIndex];
		size_t sizeAfter = oldStorage.GetEntitySize() - (sizeBefore + componentInfo.Size);

		size_t newEntityIndex = newStorage.AddEntity(entityRecord.RegistryIndex);

		uint8_t* newEntityData = newStorage.GetEntityData(newEntityIndex);
		uint8_t* oldEntityData = oldStorage.GetEntityData(entityRecord.BufferIndex);

		componentInfo.Deleter(oldEntityData + sizeBefore);

		std::memcpy(newEntityData, oldEntityData, sizeBefore);
		std::memcpy(newEntityData + sizeBefore, oldEntityData + sizeBefore + componentInfo.Size, sizeAfter);

		RemoveEntityData(entityRecord.Archetype, entityRecord.BufferIndex);

		entityRecord.Archetype = newArchetypeId;
		entityRecord.BufferIndex = newEntityIndex;

		return true;
	}

	bool Registry::IsEntityAlive(Entity entity) const
	{
		return FindEntity(entity) != m_EntityToRecord.end();
	}

	ArchetypeId Registry::GetEntityArchetype(Entity entity)
	{
		auto it = m_EntityToRecord.find(entity);
		Grapple_CORE_ASSERT(it != m_EntityToRecord.end());

		return m_EntityRecords[it->second].Archetype;
	}

	const std::vector<EntityRecord>& Registry::GetEntityRecords() const
	{
		return m_EntityRecords;
	}

	std::optional<Entity> Registry::FindEntityByIndex(uint32_t entityIndex)
	{
		auto it = m_EntityToRecord.find(Entity(entityIndex, 0));
		if (it == m_EntityToRecord.end())
			return {};
		return it->first;
	}

	std::optional<Entity> Registry::FindEntityByRegistryIndex(uint32_t registryIndex)
	{
		if (registryIndex < m_EntityRecords.size())
			return m_EntityRecords[registryIndex].Id;
		return {};
	}

	std::optional<uint8_t*> Registry::GetEntityData(Entity entity)
	{
		auto it = FindEntity(entity);
		if (it == m_EntityToRecord.end())
			return {};

		EntityRecord& record = m_EntityRecords[it->second];
		return GetEntityStorage(record.Archetype).GetEntityData(record.BufferIndex);
	}

	std::optional<const uint8_t*> Registry::GetEntityData(Entity entity) const
	{
		auto it = FindEntity(entity);
		if (it == m_EntityToRecord.end())
			return {};

		const EntityRecord& record = m_EntityRecords[it->second];
		const EntityStorage& storage = GetEntityStorage(record.Archetype);
		return storage.GetEntityData(record.BufferIndex);
	}

	std::optional<size_t> Registry::GetEntityDataSize(Entity entity) const
	{
		auto it = FindEntity(entity);
		if (it == m_EntityToRecord.end())
			return {};

		const EntityRecord& record = m_EntityRecords[it->second];
		return GetEntityStorage(record.Archetype).GetEntitySize();
	}

	std::optional<void*> Registry::GetEntityComponent(Entity entity, ComponentId component)
	{
		auto it = FindEntity(entity);
		if (it == m_EntityToRecord.end())
			return {};

		EntityRecord& entityRecord = m_EntityRecords[it->second];
		ArchetypeRecord& archetype = m_Archetypes.Records[entityRecord.Archetype];
		const EntityStorage& storage = GetEntityStorage(entityRecord.Archetype);

		std::optional<size_t> componentIndex = m_Archetypes.GetArchetypeComponentIndex(entityRecord.Archetype, component);
		if (!componentIndex.has_value())
			return {};

		uint8_t* entityData = storage.GetEntityData(entityRecord.BufferIndex);
		return entityData + archetype.ComponentOffsets[componentIndex.value()];
	}

	void Registry::RegisterComponents()
	{
		auto& initializers = ComponentInitializer::GetInitializers();
		for (ComponentInitializer* initializer : initializers)
			initializer->m_Id = RegisterComponent(*initializer);
	}

	ComponentId Registry::RegisterComponent(std::string_view name, size_t size, const std::function<void(void*)>& deleter)
	{
		Entity entityId = m_EntityIndex.CreateId();

		uint32_t registryIndex = (uint32_t)m_RegisteredComponents.size();
		ComponentInfo& info = m_RegisteredComponents.emplace_back();

		info.Id = ComponentId(entityId.GetIndex(), entityId.GetGeneration());
		info.RegistryIndex = registryIndex;
		info.Name = name;
		info.Size = size;
		info.Deleter = deleter;

		m_ComponentNameToIndex.emplace(info.Name, registryIndex);
		m_ComponentIdToIndex.emplace(info.Id, registryIndex);
		return info.Id;
	}

	std::optional<ComponentId> Registry::FindComponnet(std::string_view name)
	{
		auto it = m_ComponentNameToIndex.find(std::string(name));
		if (it == m_ComponentNameToIndex.end())
			return {};
		Grapple_CORE_ASSERT(it->second < m_RegisteredComponents.size());
		return m_RegisteredComponents[it->second].Id;
	}

	bool Registry::IsComponentIdValid(ComponentId id) const
	{
		auto it = m_ComponentIdToIndex.find(id);
		if (it == m_ComponentIdToIndex.end())
			return false;
		return it->second < m_RegisteredComponents.size();
	}

	const ComponentInfo& Registry::GetComponentInfo(ComponentId id) const
	{
		Grapple_CORE_ASSERT(IsComponentIdValid(id));
		return m_RegisteredComponents[m_ComponentIdToIndex.at(id)];
	}

	const std::vector<ComponentInfo>& Registry::GetRegisteredComponents() const
	{
		return m_RegisteredComponents;
	}

	std::optional<void*> Registry::GetSingletonComponent(ComponentId id) const
	{
		Grapple_CORE_ASSERT(IsComponentIdValid(id));

		auto it = m_Archetypes.ComponentToArchetype.find(id);
		if (it == m_Archetypes.ComponentToArchetype.end())
		{
			Grapple_CORE_ERROR("Failed to get singleton component: World doesn't contain any entities with component '{0}'", GetComponentInfo(id).Name);
			return nullptr;
		}

		const auto& archetypes = it->second;
		
		ArchetypeId archetype = INVALID_ARCHETYPE_ID;
		size_t componentIndex = SIZE_MAX;
		for (const auto& pair : archetypes)
		{
			const EntityStorage& storage = GetEntityStorage(pair.first);
			if (storage.GetEntitiesCount() != 0)
			{
				if (archetype == INVALID_ARCHETYPE_ID)
				{
					archetype = pair.first;
					componentIndex = pair.second;
				}
				else
				{
					Grapple_CORE_ERROR("Failed to get singleton component: World contains multiple entities with component '{0}'", GetComponentInfo(id).Name);
					return {};
				}
			}
		}

		if (archetype == INVALID_ARCHETYPE_ID)
		{
			Grapple_CORE_ERROR("Failed to get singleton component: World doesn't contain any entities with component '{0}'", GetComponentInfo(id).Name);
			return {};
		}

		const ArchetypeRecord& record = m_Archetypes[archetype];
		const EntityStorage& storage = GetEntityStorage(archetype);

		if (storage.GetEntitiesCount() != 1)
		{
			Grapple_CORE_ERROR("Failed to get singleton component: World contains multiple entities with component '{0}'", GetComponentInfo(id).Name);
			return {};
		}

		uint8_t* entityData = storage.GetEntityData(0);
		return entityData + record.ComponentOffsets[componentIndex];
	}

	std::optional<Entity> Registry::GetSingletonEntity(const Query& query) const
	{
		const auto& archetypes = query.GetMatchedArchetypes();

		ArchetypeId archetype = INVALID_ARCHETYPE_ID;
		size_t componentIndex = SIZE_MAX;
		for (const auto& pair : archetypes)
		{
			const EntityStorage& storage = GetEntityStorage(pair);
			if (storage.GetEntitiesCount() != 0)
			{
				if (archetype == INVALID_ARCHETYPE_ID)
					archetype = pair;
				else
				{
					Grapple_CORE_ERROR("Failed to get singleton entity: Multiple entities matched the query");
					return Entity();
				}
			}
		}

		if (archetype == INVALID_ARCHETYPE_ID)
		{
			Grapple_CORE_ERROR("Failed to get singleton entity: Zero entities matched the query");
			return Entity();
		}

		const ArchetypeRecord& record = m_Archetypes[archetype];
		const EntityStorage& storage = GetEntityStorage(archetype);
		if (storage.GetEntitiesCount() != 1)
		{
			Grapple_CORE_ERROR("Failed to get singleton entity: Multiple entities matched the query");
			return Entity();
		}

		return m_EntityRecords[storage.GetEntityIndices()[0]].Id;
	}

	EntityRegistryIterator Registry::begin()
	{
		return EntityRegistryIterator(*this, 0);
	}

	EntityRegistryIterator Registry::end()
	{
		return EntityRegistryIterator(*this, m_EntityRecords.size());
	}

	const std::vector<ComponentId>& Registry::GetEntityComponents(Entity entity)
	{
		auto it = FindEntity(entity);
		Grapple_CORE_ASSERT(it != m_EntityToRecord.end());
		return m_Archetypes.Records[m_EntityRecords[it->second].Archetype].Components;
	}

	bool Registry::HasComponent(Entity entity, ComponentId component) const
	{
		auto it = FindEntity(entity);
		Grapple_CORE_ASSERT(it != m_EntityToRecord.end());
		return m_Archetypes.GetArchetypeComponentIndex(m_EntityRecords[it->second].Archetype, component).has_value();
	}

	EntityRecord& Registry::operator[](size_t index)
	{
		Grapple_CORE_ASSERT(index < m_EntityRecords.size());
		return m_EntityRecords[index];
	}

	const EntityRecord& Registry::operator[](size_t index) const
	{
		Grapple_CORE_ASSERT(index < m_EntityRecords.size());
		return m_EntityRecords[index];
	}

	ComponentId Registry::RegisterComponent(ComponentInitializer& initializer)
	{
		Entity entityId = m_EntityIndex.CreateId();

		uint32_t registryIndex = (uint32_t)m_RegisteredComponents.size();
		ComponentInfo& info = m_RegisteredComponents.emplace_back();

		info.Id = ComponentId(entityId.GetIndex(), entityId.GetGeneration());
		info.RegistryIndex = registryIndex;
		info.Name = initializer.Type.TypeName;
		info.Size = initializer.Type.Size;
		info.Deleter = initializer.Type.Destructor;
		info.Initializer = &initializer;

		m_ComponentNameToIndex.emplace(info.Name, registryIndex);
		m_ComponentIdToIndex.emplace(info.Id, registryIndex);
		return info.Id;
	}

	ArchetypeId Registry::CreateArchetype()
	{
		ArchetypeId id = m_Archetypes.Records.size();
		ArchetypeRecord& record = m_Archetypes.Records.emplace_back();
		record.Id = id;

		m_EntityStorages.emplace_back();
		return id;
	}

	EntityStorage& Registry::GetEntityStorage(ArchetypeId archetype)
	{
		Grapple_CORE_ASSERT(m_Archetypes.IsIdValid(archetype));
		return m_EntityStorages[archetype];
	}

	const EntityStorage& Registry::GetEntityStorage(ArchetypeId archetype) const
	{
		Grapple_CORE_ASSERT(m_Archetypes.IsIdValid(archetype));
		return m_EntityStorages[archetype];
	}

	void Registry::RemoveEntityData(ArchetypeId archetype, size_t entityBufferIndex)
	{
		ArchetypeRecord& archetypeRecord = m_Archetypes.Records[archetype];

		EntityStorage& storage = GetEntityStorage(archetype);
		EntityRecord& lastEntityRecord = m_EntityRecords[storage.GetEntityIndices().back()];

		storage.RemoveEntityData(entityBufferIndex);
		lastEntityRecord.BufferIndex = entityBufferIndex;
	}

	std::unordered_map<Entity, size_t>::iterator Registry::FindEntity(Entity entity)
	{
		auto it = m_EntityToRecord.find(entity);

		if (it == m_EntityToRecord.end())
			return it;

		if (it->first != entity)
			return m_EntityToRecord.end();

		return it;
	}

	std::unordered_map<Entity, size_t>::const_iterator Registry::FindEntity(Entity entity) const
	{
		auto it = m_EntityToRecord.find(entity);

		if (it == m_EntityToRecord.cend())
			return it;

		if (it->first != entity)
			return m_EntityToRecord.cend();

		return it;
	}
}