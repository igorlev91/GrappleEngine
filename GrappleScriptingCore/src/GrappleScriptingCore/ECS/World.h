#pragma once

#include "GrappleECS/ComponentId.h"
#include "GrappleECS/System.h"
#include "GrappleECS/EntityId.h"

#include "GrappleECS/ComponentGroup.h"

#include "GrappleScriptingCore/ECS/Query.h"
#include "GrappleScriptingCore/ECS/EntityView.h"
#include "GrappleScriptingCore/Bindings.h"

#include <array>
#include <optional>
#include <string_view>
#include <functional>

#include <stdint.h>

namespace Grapple::Scripting
{
	class World
	{
	public:
		static constexpr std::optional<SystemGroupId> FindSystemGroup(std::string_view name)
		{
			return Bindings::Instance->FindSystemGroup(name);
		}

		template<typename... Components>
		static constexpr Entity CreateEntity()
		{
			ComponentGroup<Components...> group;
			return Bindings::Instance->CreateEntity(group.GetIds().data(), group.GetIds().size());
		}

		template<typename ComponentT>
		inline static ComponentT& GetEntityComponent(Entity entity)
		{
			return *(ComponentT*)Bindings::Instance->GetEntityComponent(entity, ComponentT::Info.Id);
		}

		constexpr static Entity GetSingletonEntity(Query query)
		{
			return Bindings::Instance->GetSingletonEntity(query.GetId());
		}

		template<typename T>
		constexpr static T* GetSingletonComponent()
		{
			return (T*)Bindings::Instance->GetSingletonComponent(T::Info.Id);
		}

		static constexpr bool IsAlive(Entity entity)
		{
			return Bindings::Instance->IsEntityAlive(entity);
		}

		static constexpr void ForEachChunk(Query query, const std::function<void(EntityView&)>& perChunk)
		{
			Bindings::Instance->ForEachChunkInQuery(query.GetId(), perChunk);
		}
	};
}