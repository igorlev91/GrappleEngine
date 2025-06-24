#include "CommandBuffer.h"

#include "GrappleCore/Profiler/Profiler.h"

#include "GrappleECS/World.h"

namespace Grapple
{
	EntitiesCommandBuffer::EntitiesCommandBuffer(World& world)
		: m_Storage(2048), m_World(world)
	{

	}

	FutureEntityCommands EntitiesCommandBuffer::GetEntity(Entity entity)
	{
		return AddEntityCommand<GetEntityCommand>(GetEntityCommand(entity));
	}

	void EntitiesCommandBuffer::DeleteEntity(Entity entity)
	{
		AddCommand<DeleteEntityCommand>(DeleteEntityCommand(entity));
	}

	void EntitiesCommandBuffer::Execute()
	{
		Grapple_PROFILE_FUNCTION();
		if (!m_Storage.CanRead())
			return;

		while (m_Storage.CanRead())
		{
			auto [meta, command] = m_Storage.Pop();

			CommandContext context(meta, m_Storage);

			command->Apply(context, m_World);
			command->~Command();
		}

		m_Storage.Clear();
	}
}
