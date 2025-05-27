#pragma once

#include "GrappleCore/Assert.h"

#include "GrappleECS/Entity/Component.h"
#include "GrappleECS/Entity/Archetype.h"
#include "GrappleECS/Entity/Archetypes.h"

#include "GrappleECS/Query/QueryData.h"

#include <vector>
#include <unordered_map>

namespace Grapple
{
	class GrappleECS_API Entities;
	class GrappleECS_API Query;

	class GrappleECS_API QueryCache
	{
	public:
		QueryCache(Entities& entities, Archetypes& archetypes)
			: m_Entities(entities), m_Archetypes(archetypes) {}

		~QueryCache();

		QueryCache(const QueryCache&) = delete;
		QueryCache& operator=(const QueryCache&) = delete;

		const QueryData& operator[](QueryId id) const;

		inline const Entities& GetEntitites() const { return m_Entities; }
		inline Entities& GetEntitites() { return m_Entities; }
	public:
		Query CreateQuery(QueryCreationData& creationData);

		void OnArchetypeCreated(ArchetypeId archetype);
	private:
		bool CompareComponentSets(const std::vector<ComponentId>& archetypeComponents, const std::vector<ComponentId>& queryComponents);
	private:
		Entities& m_Entities;
		Archetypes& m_Archetypes;

		std::vector<QueryData> m_Queries;
		std::unordered_map<ComponentId, std::vector<QueryId>> m_CachedMatches;
	};
}