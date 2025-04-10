#pragma once

#include "GrappleECS/Registry.h"
#include "GrappleECS/Entity/Component.h"

#include "GrappleECS/ComponentGroup.h"

#include "GrappleECS/QueryFilters.h"
#include "GrappleECS/Query/Query.h"

#include "GrappleECS/System/SystemsManager.h"

#include <vector>
#include <string_view>

#define Grapple_COMPONENT static Grapple::RegisteredComponentInfo Info;
#define Grapple_COMPONENT_IMPL(name) Grapple::RegisteredComponentInfo name::Info;

namespace Grapple
{
	class World
	{
	public:
		World()
		{
			Grapple_CORE_ASSERT(s_CurrentWorld == nullptr, "Multiple ECS Worlds");
			s_CurrentWorld = this;
		}

		~World()
		{
			s_CurrentWorld = nullptr;
		}

		World(const World&) = delete;

		template<typename ComponentT>
		constexpr void RegisterComponent()
		{
			ComponentT::Info.Id = m_Registry.RegisterComponent(typeid(ComponentT).name(), 
				sizeof(ComponentT), [](void* component) { ((ComponentT*)component)->~ComponentT(); });
		}

		template<typename... T>
		constexpr Entity CreateEntity()
		{
			ComponentId ids[sizeof...(T)];

			size_t index = 0;
			([&]
			{
				ids[index++] = T::Info.Id;
			} (), ...);

			return m_Registry.CreateEntity(ComponentSet(ids, sizeof...(T)));
		}

		template<typename T>
		constexpr T& GetEntityComponent(Entity entity)
		{
			std::optional<void*> componentData = m_Registry.GetEntityComponent(entity, T::Info.Id);
			Grapple_CORE_ASSERT(componentData.has_value(), "Failed to get entity component");
			return *(T*)componentData.value();
		}

		template<typename T>
		constexpr std::optional<T*> TryGetEntityComponent(Entity entity)
		{
			std::optional<void*> componentData = m_Registry.GetEntityComponent(entity, T::Info.Id);
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
			return m_Registry.AddEntityComponent(entity, T::Info.Id, &data);
		}

		template<typename T>
		constexpr bool RemoveEntityComponent(Entity entity)
		{
			return m_Registry.RemoveEntityComponent(entity, T::Info.Id);
		}
		
		template<typename T>
		constexpr bool HasComponent(Entity entity)
		{
			return m_Registry.HasComponent(entity, T::Info.Id);
		}

		inline void DeleteEntity(Entity entity)
		{
			m_Registry.DeleteEntity(entity);
		}

		inline bool IsEntityAlive(Entity entity) const
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
			FilteredComponentsGroup<T...> components;
			return m_Registry.CreateQuery(ComponentSet(components.GetComponents().data(), components.GetComponents().size()));
		}

		inline static World& GetCurrent()
		{
			Grapple_CORE_ASSERT(s_CurrentWorld != nullptr);
			return *s_CurrentWorld;
		}
	public:
		inline Registry& GetRegistry() { return m_Registry; }

		SystemsManager& GetSystemsManager() { return m_SystemsManager; }
		const SystemsManager& GetSystemsManager() const { return m_SystemsManager; }
	private:
		Registry m_Registry;

		SystemsManager m_SystemsManager;
	private:
		static World* s_CurrentWorld;
	};
}