#pragma once

#include "Grapple/Core/Core.h"

#include "GrappleECS/Commands/CommandsStorage.h"
#include "GrappleECS/Commands/Command.h"
#include "GrappleECS/Commands/Commands.h"
#include "GrappleECS/World.h"

#include <type_traits>

namespace Grapple
{
	class GrappleECS_API CommandBuffer
	{
	public:
		CommandBuffer();

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
		void RemoveComponent(Entity entity)
		{
			AddCommand<RemoveComponentCommand>(RemoveComponentCommand(entity, COMPONENT_ID(T)));
		}

		void Execute(World& world);
	private:
		CommandsStorage m_Storage;
	};
}