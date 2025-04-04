#include "Registry.h"

#include "GrappleECS/Query/EntityView.h"
#include "GrappleECS/Query/EntityRegistryIterator.h"

#include "GrappleECS/Query/Query.h"

#include "GrappleECS/EntityStorage/EntityChunksPool.h"

#include <algorithm>

namespace Grapple
{
	Registry::Registry()
		: m_QueryCache(*this)
	{
		EntityChunksPool::Initialize(16);
	}

	Registry::~Registry()
	{
		for (const ArchetypeRecord& archetype : m_Archetypes)
		{
			for (size_t entityIndex = 0; entityIndex < archetype.Storage.GetEntitiesCount(); entityIndex++)
			{
				uint8_t* entityData = archetype.Storage.GetEntityData(entityIndex);
				for (size_t i = 0; i < archetype.Data.Components.size(); i++)
				{
					GetComponentInfo(archetype.Data.Components[i]).Deleter((void*)archetype.Data.ComponentOffsets[i]);
				}
			}
		}
	}

	Entity Registry::CreateEntity(const ComponentSet& componentSet)
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

		auto it = m_ComponentSetToArchetype.find(components);
		if (it != m_ComponentSetToArchetype.end())
			record.Archetype = it->second;
		else
		{
			ArchetypeId newArchetypeId = CreateArchetype();
			ArchetypeRecord& newArchetype = m_Archetypes[newArchetypeId];
			record.Archetype = newArchetype.Data.Id;

			newArchetype.Data.ComponentOffsets.resize(components.GetCount());
			newArchetype.Data.Components.resize(components.GetCount());

			std::memcpy(newArchetype.Data.Components.data(), components.GetIds(), components.GetCount() * sizeof(size_t));

			size_t entitySize = 0;
			for (size_t i = 0; i < components.GetCount(); i++)
			{
				newArchetype.Data.ComponentOffsets[i] = entitySize;
				entitySize += GetComponentInfo(components[i]).Size;
			}

			newArchetype.Storage.SetEntitySize(entitySize);

			m_ComponentSetToArchetype.emplace(ComponentSet(newArchetype.Data.Components), newArchetype.Data.Id);

			for (size_t i = 0; i < newArchetype.Data.Components.size(); i++)
				m_ComponentToArchetype[newArchetype.Data.Components[i]].emplace(newArchetypeId, i);

			m_QueryCache.OnArchetypeCreated(newArchetypeId);
		}

		ArchetypeRecord& archetypeRecord = m_Archetypes[record.Archetype];
		record.BufferIndex = archetypeRecord.Storage.AddEntity(record.RegistryIndex);

		m_EntityToRecord.emplace(record.Id, record.RegistryIndex);

		uint8_t* entityData = archetypeRecord.Storage.GetEntityData(record.BufferIndex);
		// Intialize entity data to 0
		std::memset(entityData, 0, archetypeRecord.Storage.GetEntitySize());
		return record.Id;
	}

	void Registry::DeleteEntity(Entity entity)
	{
		auto recordIterator = FindEntity(entity);
		if (recordIterator == m_EntityToRecord.end())
			return;

		EntityRecord& record = m_EntityRecords[recordIterator->second];
		EntityRecord& lastEntityRecord = m_EntityRecords.back();

		m_EntityIndex.AddDeletedId(record.Id);

		// Move last entity in the registry to the location of a deleted entity
		m_Archetypes[lastEntityRecord.Archetype].Storage.UpdateEntityRegistryIndex(lastEntityRecord.BufferIndex, record.RegistryIndex);
		RemoveEntityData(record.Archetype, record.BufferIndex);

		record.Id = lastEntityRecord.Id;
		record.Archetype = lastEntityRecord.Archetype;
		record.BufferIndex = lastEntityRecord.BufferIndex;
		m_EntityToRecord[lastEntityRecord.Id] = record.RegistryIndex;

		m_EntityToRecord.erase(recordIterator);
		m_EntityRecords.erase(m_EntityRecords.end() - 1);
	}

	bool Registry::AddEntityComponent(Entity entity, ComponentId componentId, const void* componentData)
	{
		Grapple_CORE_ASSERT(IsComponentIdValid(componentId), "Invalid component id");

		const ComponentInfo& componentInfo = GetComponentInfo(componentId);

		auto recordIterator = FindEntity(entity);
		if (recordIterator == m_EntityToRecord.end())
			return {};

		EntityRecord& entityRecord = m_EntityRecords[recordIterator->second];
		ArchetypeRecord& archetype = m_Archetypes[entityRecord.Archetype];

		// Can only have one instance of a component
		if (GetArchetypeComponentIndex(entityRecord.Archetype, componentId).has_value())
			return false;

		size_t oldComponentCount = archetype.Data.Components.size();
		ArchetypeId newArchetypeId = INVALID_ARCHETYPE_ID;

		size_t insertedComponentIndex = SIZE_MAX;

		auto edgeIterator = archetype.Data.Edges.find(componentId);
		if (edgeIterator != archetype.Data.Edges.end())
		{
			newArchetypeId = edgeIterator->second.Add;
			
			std::optional<size_t> index = GetArchetypeComponentIndex(newArchetypeId, componentId);
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

			std::memcpy(newComponents.data(), archetype.Data.Components.data(), oldComponentCount * sizeof(componentId));
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
			auto it = m_ComponentSetToArchetype.find(ComponentSet(newComponents));

			if (it != m_ComponentSetToArchetype.end())
			{
				newArchetypeId = it->second;
			}
			else
			{
				newArchetypeId = CreateArchetype();

				ArchetypeRecord& newArchetype = m_Archetypes[newArchetypeId];
				newArchetype.Data.Components = std::move(newComponents);
				newArchetype.Data.ComponentOffsets.resize(newArchetype.Data.Components.size());

				size_t entitySize = 0;
				for (size_t i = 0; i < newArchetype.Data.Components.size(); i++)
				{
					newArchetype.Data.ComponentOffsets[i] = entitySize;
					entitySize += GetComponentInfo(newArchetype.Data.Components[i]).Size;
				}

				newArchetype.Storage.SetEntitySize(entitySize);
				m_ComponentSetToArchetype.emplace(ComponentSet(newArchetype.Data.Components), newArchetypeId);

				shouldNotifyQueryCache = true;
			}

			m_Archetypes[entityRecord.Archetype].Data.Edges.emplace(componentId, ArchetypeEdge{newArchetypeId, INVALID_ARCHETYPE_ID});
			m_Archetypes[newArchetypeId].Data.Edges.emplace(componentId, ArchetypeEdge{INVALID_ARCHETYPE_ID, entityRecord.Archetype});

			ArchetypeRecord& newArchetype = m_Archetypes[newArchetypeId];
			for (size_t i = 0; i < newArchetype.Data.Components.size(); i++)
				m_ComponentToArchetype[newArchetype.Data.Components[i]].emplace(newArchetypeId, i);

			if (shouldNotifyQueryCache)
				m_QueryCache.OnArchetypeCreated(newArchetypeId);
		}

		Grapple_CORE_ASSERT(insertedComponentIndex != SIZE_MAX);

		ArchetypeRecord& oldArchetype = m_Archetypes[entityRecord.Archetype];
		ArchetypeRecord& newArchetype = m_Archetypes[newArchetypeId];

		size_t newEntityIndex = newArchetype.Storage.AddEntity(entityRecord.RegistryIndex);
		uint8_t* newEntityData = newArchetype.Storage.GetEntityData(newEntityIndex);
		const uint8_t* oldEntityData = oldArchetype.Storage.GetEntityData(entityRecord.BufferIndex);

		size_t sizeBefore = oldArchetype.Storage.GetEntitySize();
		if (insertedComponentIndex < oldArchetype.Data.Components.size())
			sizeBefore = oldArchetype.Data.ComponentOffsets[insertedComponentIndex];

		size_t sizeAfter = oldArchetype.Storage.GetEntitySize() - sizeBefore;

		std::memcpy(newEntityData, oldEntityData, sizeBefore);
		std::memcpy(newEntityData + sizeBefore + componentInfo.Size, oldEntityData + sizeBefore, sizeAfter);

		if (componentData == nullptr)
			std::memset(newEntityData + sizeBefore, 0, componentInfo.Size);
		else
			std::memcpy(newEntityData + sizeBefore, componentData, componentInfo.Size);

		RemoveEntityData(entityRecord.Archetype, entityRecord.BufferIndex);

		entityRecord.Archetype = newArchetypeId;
		entityRecord.BufferIndex = newEntityIndex;
	}

	bool Registry::RemoveEntityComponent(Entity entity, ComponentId componentId)
	{
		Grapple_CORE_ASSERT(IsComponentIdValid(componentId), "Invalid component id");

		const ComponentInfo& componentInfo = GetComponentInfo(componentId);

		auto recordIterator = FindEntity(entity);
		if (recordIterator == m_EntityToRecord.end())
			return {};

		EntityRecord& entityRecord = m_EntityRecords[recordIterator->second];
		ArchetypeRecord& archetype = m_Archetypes[entityRecord.Archetype];

		size_t removedComponentIndex = SIZE_MAX;
		ArchetypeId newArchetypeId = INVALID_ARCHETYPE_ID;
		
		{
			std::optional<size_t> componentIndex = GetArchetypeComponentIndex(entityRecord.Archetype, componentId);
			if (componentIndex.has_value())
				removedComponentIndex = componentIndex.value();
			else
				return false;
		}

		auto edgeIterator = archetype.Data.Edges.find(componentId);
		if (edgeIterator != archetype.Data.Edges.end())
		{
			newArchetypeId = edgeIterator->second.Remove;
		}
		else
		{
			size_t oldComponentCount = archetype.Data.Components.size();
			std::vector<ComponentId> newComponents(oldComponentCount - 1);

			for (size_t insertIndex = 0, i = 0; i < oldComponentCount; i++)
			{
				if (archetype.Data.Components[i] == componentId)
					continue;
				else
				{
					newComponents[insertIndex] = archetype.Data.Components[i];
					insertIndex++;
				}
			}

			auto it = m_ComponentSetToArchetype.find(ComponentSet(newComponents));
			if (it != m_ComponentSetToArchetype.end())
			{
				newArchetypeId = it->second;
			}
			else
			{
				newArchetypeId = CreateArchetype();

				ArchetypeRecord& newArchetype = m_Archetypes[newArchetypeId];
				newArchetype.Data.Components = std::move(newComponents);
				newArchetype.Data.ComponentOffsets.resize(newArchetype.Data.Components.size());

				size_t entitySize = 0;
				for (size_t i = 0; i < newArchetype.Data.Components.size(); i++)
				{
					newArchetype.Data.ComponentOffsets[i] = entitySize;
					entitySize += GetComponentInfo(newArchetype.Data.Components[i]).Size;
				}

				newArchetype.Storage.SetEntitySize(entitySize);
				m_ComponentSetToArchetype.emplace(ComponentSet(newArchetype.Data.Components), newArchetypeId);
			}

			m_Archetypes[entityRecord.Archetype].Data.Edges.emplace(componentId, ArchetypeEdge{ INVALID_ARCHETYPE_ID, newArchetypeId});
			m_Archetypes[newArchetypeId].Data.Edges.emplace(componentId, ArchetypeEdge{ entityRecord.Archetype, INVALID_ARCHETYPE_ID });

			ArchetypeRecord& newArchetype = m_Archetypes[newArchetypeId];
			for (size_t i = 0; i < newArchetype.Data.Components.size(); i++)
				m_ComponentToArchetype[newArchetype.Data.Components[i]].emplace(newArchetypeId, i);
		}

		ArchetypeRecord& oldArchetype = m_Archetypes[entityRecord.Archetype];
		ArchetypeRecord& newArchetype = m_Archetypes[newArchetypeId];

		size_t sizeBefore = oldArchetype.Data.ComponentOffsets[removedComponentIndex];
		size_t sizeAfter = oldArchetype.Storage.GetEntitySize() - (sizeBefore + componentInfo.Size);

		size_t newEntityIndex = newArchetype.Storage.AddEntity(entityRecord.RegistryIndex);

		uint8_t* newEntityData = newArchetype.Storage.GetEntityData(newEntityIndex);
		uint8_t* oldEntityData = oldArchetype.Storage.GetEntityData(entityRecord.BufferIndex);

		componentInfo.Deleter(oldEntityData + sizeBefore);

		std::memcpy(newEntityData, oldEntityData, sizeBefore);
		std::memcpy(newEntityData + sizeBefore, oldEntityData + sizeBefore + componentInfo.Size, sizeAfter);

		RemoveEntityData(entityRecord.Archetype, entityRecord.BufferIndex);

		entityRecord.Archetype = newArchetypeId;
		entityRecord.BufferIndex = newEntityIndex;
	}

	bool Registry::IsEntityAlive(Entity entity) const
	{
		return FindEntity(entity) != m_EntityToRecord.end();
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
		return m_Archetypes[record.Archetype].Storage.GetEntityData(record.BufferIndex);
	}

	std::optional<const uint8_t*> Registry::GetEntityData(Entity entity) const
	{
		auto it = FindEntity(entity);
		if (it == m_EntityToRecord.end())
			return {};

		const EntityRecord& record = m_EntityRecords[it->second];
		return m_Archetypes[record.Archetype].Storage.GetEntityData(record.BufferIndex);
	}

	std::optional<size_t> Registry::GetEntityDataSize(Entity entity)
	{
		auto it = FindEntity(entity);
		if (it == m_EntityToRecord.end())
			return {};

		EntityRecord& record = m_EntityRecords[it->second];
		return m_Archetypes[record.Archetype].Storage.GetEntitySize();
	}

	std::optional<void*> Registry::GetEntityComponent(Entity entity, ComponentId component)
	{
		auto it = FindEntity(entity);
		if (it == m_EntityToRecord.end())
			return {};

		EntityRecord& entityRecord = m_EntityRecords[it->second];
		ArchetypeRecord& archetype = m_Archetypes[entityRecord.Archetype];

		std::optional<size_t> componentIndex = GetArchetypeComponentIndex(entityRecord.Archetype, component);
		if (!componentIndex.has_value())
			return {};

		uint8_t* entityData = archetype.Storage.GetEntityData(entityRecord.BufferIndex);
		return entityData + archetype.Data.ComponentOffsets[componentIndex.value()];
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
		auto it = m_ComponentNameToIndex.find(name);
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

	inline const ComponentInfo& Registry::GetComponentInfo(ComponentId id) const
	{
		Grapple_CORE_ASSERT(IsComponentIdValid(id));
		return m_RegisteredComponents[m_ComponentIdToIndex.at(id)];
	}

	EntityRegistryIterator Registry::begin()
	{
		return EntityRegistryIterator(*this, 0);
	}

	EntityRegistryIterator Registry::end()
	{
		return EntityRegistryIterator(*this, m_EntityRecords.size());
	}

	Query Registry::CreateQuery(const ComponentSet& components)
	{
		return m_QueryCache.AddQuery(components);
	}

	EntityView Registry::QueryArchetype(const ComponentSet& componentSet)
	{
		Grapple_CORE_ASSERT(componentSet.GetCount() > 0);

		if (m_TemporaryComponentSet.size() < componentSet.GetCount())
			m_TemporaryComponentSet.resize(componentSet.GetCount());

		std::memcpy(m_TemporaryComponentSet.data(), componentSet.GetIds(), sizeof(ComponentId) * componentSet.GetCount());

		std::sort(m_TemporaryComponentSet.data(), m_TemporaryComponentSet.data() + componentSet.GetCount());
		auto it = m_ComponentSetToArchetype.find(ComponentSet(m_TemporaryComponentSet.data(), componentSet.GetCount()));

		Grapple_CORE_ASSERT(it != m_ComponentSetToArchetype.end());

		size_t archetypeId = it->second;
		return EntityView(*this, archetypeId);
	}

	const std::vector<ComponentId>& Registry::GetEntityComponents(Entity entity)
	{
		auto it = FindEntity(entity);
		Grapple_CORE_ASSERT(it != m_EntityToRecord.end());
		return m_Archetypes[m_EntityRecords[it->second].Archetype].Data.Components;
	}

	bool Registry::HasComponent(Entity entity, ComponentId component) const
	{
		auto it = FindEntity(entity);
		Grapple_CORE_ASSERT(it != m_EntityToRecord.end());
		return GetArchetypeComponentIndex(m_EntityRecords[it->second].Archetype, component).has_value();
	}

	inline EntityRecord& Registry::operator[](size_t index)
	{
		Grapple_CORE_ASSERT(index < m_EntityRecords.size());
		return m_EntityRecords[index];
	}

	inline const EntityRecord& Registry::operator[](size_t index) const
	{
		Grapple_CORE_ASSERT(index < m_EntityRecords.size());
		return m_EntityRecords[index];
	}

	ArchetypeId Registry::CreateArchetype()
	{
		ArchetypeId id = m_Archetypes.size();
		ArchetypeRecord& record = m_Archetypes.emplace_back();
		record.Data.Id = id;
		
		return id;
	}

	std::optional<size_t> Registry::GetArchetypeComponentIndex(ArchetypeId archetype, ComponentId component) const
	{
		auto it = m_ComponentToArchetype.find(component);
		if (it != m_ComponentToArchetype.end())
		{
			auto it2 = it->second.find(archetype);
			if (it2 != it->second.end())
			{
				return { it2->second };
			}
			else
				return {};
		}
		else
			return {};
	}

	void Registry::RemoveEntityData(ArchetypeId archetype, size_t entityBufferIndex)
	{
		ArchetypeRecord& archetypeRecord = m_Archetypes[archetype];
		EntityRecord& lastEntityRecord = m_EntityRecords[archetypeRecord.Storage.GetEntityIndices().back()];

		archetypeRecord.Storage.RemoveEntityData(entityBufferIndex);
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