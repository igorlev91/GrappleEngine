#pragma once

#include "GrappleCore/Core.h"

#include "GrappleECS/Commands/CommandsStorage.h"
#include "GrappleECS/Commands/Command.h"
#include "GrappleECS/Commands/Commands.h"

#include <type_traits>

namespace Grapple
{
	class GrappleECS_API World;
	class GrappleECS_API CommandBuffer
	{
	public:
		CommandBuffer(World& world);

		template<typename T>
		void AddCommand(const T& command)
		{
			static_assert(std::is_base_of_v<Command, T> == true, "T is not a Command");

			CommandMetadata metadata;
			metadata.Apply = [](Command* c, World& world) { ((T*)c)->Apply(world); };
			metadata.CommandSize = sizeof(T);

			m_Storage.Push(metadata, (const void*) &command);
		}

		template<typename T>
		void AddComponent(Entity entity, ComponentInitializationStrategy initStrategy = ComponentInitializationStrategy::DefaultConstructor)
		{
			AddCommand<AddComponentCommand>(AddComponentCommand(entity, COMPONENT_ID(T), initStrategy));
		}

		template<typename T>
		void AddComponentWithData(Entity entity, const T& component)
		{
			AddCommand<AddComponentWithDataCommand<T>>(AddComponentWithDataCommand<T>(entity, component));
		}

		template<typename T>
		void RemoveComponent(Entity entity)
		{
			AddCommand<RemoveComponentCommand>(RemoveComponentCommand(entity, COMPONENT_ID(T)));
		}

		template<typename... T>
		void CreateEntity(ComponentInitializationStrategy initStrategy = ComponentInitializationStrategy::DefaultConstructor)
		{
			AddCommand<CreateEntityCommand<T...>>(CreateEntityCommand<T...>(initStrategy));
		}

		template<typename... T>
		void CreateEntity(const T& ...components)
		{
			AddCommand<CreateEntityWithDataCommand<T...>>(CreateEntityWithDataCommand<T...>(components...));
		}

		void DeleteEntity(Entity entity);

		void Execute();
	private:
		World& m_World;
		CommandsStorage m_Storage;
	};
}