#pragma once

#include "Grapple/Core/Assert.h"
#include "Grapple/Core/Core.h"

#include <string>
#include <vector>
#include <xhash>
#include <functional>

namespace Grapple
{
	using ComponentId = size_t;
	constexpr ComponentId INVALID_COMPONENT_ID = SIZE_MAX;

	struct ComponentInfo
	{
		ComponentId Id;
		std::string Name;
		size_t Size;

		std::function<void(void*)> Deleter;
	};

	class ComponentSetIterator
	{
	public:
		constexpr ComponentSetIterator(const ComponentId* element)
			: m_Element(element) {}

		constexpr ComponentId operator*() const
		{
			return *m_Element;
		}

		constexpr ComponentSetIterator operator++()
		{
			++m_Element;
			return *this;
		}

		constexpr bool operator==(ComponentSetIterator other)
		{
			return m_Element == other.m_Element;
		}

		constexpr bool operator!=(ComponentSetIterator other)
		{
			return m_Element != other.m_Element;
		}
	private:
		const ComponentId* m_Element;
	};

	class ComponentSet
	{
	public:
		ComponentSet(std::vector<ComponentId>& ids)
			: m_Ids(ids.data()), m_Count(ids.size())
		{
			Grapple_CORE_ASSERT(m_Count, "Components count shouldn't been 0");
		}

		constexpr ComponentSet(ComponentId* ids, size_t count)
			: m_Ids(ids), m_Count(count) {}

		constexpr ComponentId* GetIds() { return m_Ids; }
		constexpr const ComponentId* GetIds() const { return m_Ids; }
		constexpr size_t GetCount() const { return m_Count; }

		constexpr ComponentId operator[](size_t index) const
		{
			Grapple_CORE_ASSERT(index < m_Count);
			return m_Ids[index];
		}

		constexpr ComponentSetIterator begin() const
		{
			return ComponentSetIterator(m_Ids);
		}

		constexpr ComponentSetIterator end() const
		{
			return ComponentSetIterator(m_Ids + m_Count);
		}
	private:
		ComponentId* m_Ids;
		size_t m_Count;
	};

	bool operator==(const ComponentSet& setA, const ComponentSet& setB);
	bool operator!=(const ComponentSet& setA, const ComponentSet& setB);
}

template<>
struct std::hash<Grapple::ComponentSet>
{
	size_t operator()(const Grapple::ComponentSet& set) const
	{
		size_t hash = 0;
		for (size_t i = 0; i < set.GetCount(); i++)
			Grapple::CombineHashes<Grapple::ComponentId>(hash, set[i]);

		return hash;
	}
};