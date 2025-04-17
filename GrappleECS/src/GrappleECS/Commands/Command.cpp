#include "Command.h"

#include "GrappleECS/Commands/CommandsStorage.h"

namespace Grapple
{
    Entity CommandContext::GetEntity(FutureEntity futureEntity)
    {
        auto entity = m_Storage.Read<Entity>(futureEntity.Location);
        Grapple_CORE_ASSERT(entity.has_value());

        return *entity.value();
    }

    void CommandContext::SetEntity(FutureEntity futureEntity, Entity entity)
    {
        Grapple_CORE_ASSERT(m_Storage.Write<Entity>(futureEntity.Location, entity));
    }
}
