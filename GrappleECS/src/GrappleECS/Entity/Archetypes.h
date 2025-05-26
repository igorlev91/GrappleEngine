#pragma once

#include "GrappleCore/Assert.h"

#include "GrappleECS/Entity/Archetype.h"

#include <vector>
#include <unordered_map>

namespace Grapple
{
	struct GrappleECS_API Archetypes
	{
		Archetypes() = default;
		~Archetypes();

		Archetypes(const Archetypes&) = delete;
		Archetypes& operator=(const Archetypes&) = delete;

		inline void Clear()
		{
			ComponentSetToArchetype.clear();
			ComponentToArchetype.clear();

			Records.clear();
		}

		inline bool IsIdValid(ArchetypeId id) const
		{
			return (size_t)id < Records.size();
		}

		inline const ArchetypeRecord& operator[](ArchetypeId id) const
		{
			Grapple_CORE_ASSERT((size_t)id < Records.size());
			return Records[(size_t)id];
		}

		std::optional<size_t> GetArchetypeComponentIndex(ArchetypeId archetype, ComponentId component) const
		{
			Grapple_CORE_ASSERT(IsIdValid(archetype));

			auto it = ComponentToArchetype.find(component);
			if (it != ComponentToArchetype.end())
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

		std::vector<ArchetypeRecord> Records;
		std::unordered_map<ComponentSet, ArchetypeId> ComponentSetToArchetype;
		std::unordered_map<ComponentId, std::unordered_map<ArchetypeId, size_t>> ComponentToArchetype;
	};
}