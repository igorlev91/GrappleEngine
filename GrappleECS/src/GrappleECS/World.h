#pragma once

#include "GrappleECS/Registry.h"
#include "GrappleECS/Entity/Component.h"

#include "GrappleECS/Query/Query.h"

#include "GrappleECS/System/System.h"

#include <vector>
#include <string_view>

#define Grapple_COMPONENT static Grapple::ComponentId Id;
#define Grapple_COMPONENT_IMPL(name) Grapple::ComponentId name::Id = INVALID_COMPONENT_ID;

namespace Grapple
{
	class QueryFilter
	{
	public:
		constexpr QueryFilter(ComponentId component)
			: Component(component) {}

		const ComponentId Component;
	};

	template<typename T>
	class With : public QueryFilter
	{
	public:
		constexpr With()
			: QueryFilter(T::Id) {}
	};

	template<typename T>
	class Without : public QueryFilter
	{
	public:
		constexpr Without()
			: QueryFilter(ComponentId(T::Id.GetIndex() | (uint32_t)QueryFilterType::Without, T::Id.GetGeneration())) {}
	};

	template<template<typename ...> class, template<typename...> class>
	struct IsSameTemplate : std::false_type {};

	template<template<typename ...> class T>
	struct IsSameTemplate<T, T> : std::true_type {};

	class World
	{
	public:
		World() = default;
		World(const World&) = delete;

		template<typename ComponentT>
		constexpr void RegisterComponent()
		{
			ComponentT::Id = m_Registry.RegisterComponent(typeid(ComponentT).name(), sizeof(ComponentT), [](void* component) { ((ComponentT*)component)->~ComponentT(); });
		}

		template<typename... T>
		constexpr Entity CreateEntity()
		{
			ComponentId ids[sizeof...(T)];

			size_t index = 0;
			([&]
			{
				ids[index++] = T::Id;
			} (), ...);

			return m_Registry.CreateEntity(ComponentSet(ids, sizeof...(T)));
		}

		template<typename T>
		constexpr T& GetEntityComponent(Entity entity)
		{
			std::optional<void*> componentData = m_Registry.GetEntityComponent(entity, T::Id);
			Grapple_CORE_ASSERT(componentData.has_value(), "Failed to get entity component");
			return *(T*)componentData.value();
		}

		template<typename T>
		constexpr std::optional<T*> TryGetEntityComponent(Entity entity)
		{
			std::optional<void*> componentData = m_Registry.GetEntityComponent(entity, T::Id);
			if (componentData.has_value())
				return (T*)componentData.value();
			return {};
		}

		template<typename T>
		constexpr std::optional<T*> TryGetEntityComponent(Entity entity, ComponentId component)
		{
			if (T::Id != component)
				return {};

			return TryGetEntityComponent<T>(entity);
		}

		template<typename T>
		constexpr bool AddEntityComponent(Entity entity, const T& data)
		{
			return m_Registry.AddEntityComponent(entity, T::Id, &data);
		}

		template<typename T>
		constexpr bool RemoveEntityComponent(Entity entity)
		{
			return m_Registry.RemoveEntityComponent(entity, T::Id);
		}
		
		template<typename T>
		constexpr bool HasComponent(Entity entity)
		{
			return m_Registry.HasComponent(entity, T::Id);
		}

		inline void DeleteEntity(Entity entity)
		{
			m_Registry.DeleteEntity(entity);
		}

		inline bool IsEntityAlive(Entity entity)
		{
			return m_Registry.IsEntityAlive(entity);
		}

		inline const std::vector<ComponentId>& GetEntityComponents(Entity entity)
		{
			return m_Registry.GetEntityComponents(entity);
		}

		template<typename... T>
		constexpr Query CreateQuery()
		{
			ComponentId ids[sizeof...(T)];
			size_t index = 0;

			([&]
			{
				T filter;
				ids[index++] = filter.Component;
			} (), ...);

			return m_Registry.CreateQuery(ComponentSet(ids, sizeof...(T)));
		}
	public:
		inline Registry& GetRegistry() { return m_Registry; }

		void RegisterSystem(const Query& query, const SystemFunction& system);

		void OnUpdate();
	private:
		Registry m_Registry;

		std::vector<System> m_Systems;
	};
}