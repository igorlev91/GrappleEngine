#pragma once

#include "Grapple/Core/Assert.h"
#include "Grapple/Core/Core.h"

#include "GrappleECS/Entity.h"
#include "GrappleECS/Component.h"
#include "GrappleECS/Archetype.h"

#include <unordered_map>
#include <vector>

namespace Grapple
{
	class Registry
	{
	public:
		Entity CreateEntity(ComponentSet& components);

		ComponentId RegisterComponent(std::string_view name, size_t size);
		inline const ComponentInfo& GetComponentInfo(size_t index) const;
	public:
		inline EntityRecord& operator[](size_t index);
		inline const EntityRecord& operator[](size_t index) const;
	private:
		std::vector<EntityRecord> m_EntityRecords;
		std::unordered_map<Entity, size_t> m_EntityToRecord;

		std::vector<ArchetypeRecord> m_Archetypes;
		std::unordered_map<ComponentSet, size_t> m_ComponentSetToArchetype;

		std::vector<ComponentInfo> m_RegisteredComponents;
	};
}