#include "Prefab.h"

#include "Grapple/AssetManager/AssetManager.h"

#include <yaml-cpp/yaml.h>

namespace Grapple
{
    Entity Prefab::CreateInstance(World& world)
    {
        Entity entity;
        if (m_Archetype == INVALID_ARCHETYPE_ID)
        {
            entity = world.Entities.CreateEntity(ComponentSet(m_Components), ComponentInitializationStrategy::Zero);
            m_Archetype = world.Entities.GetEntityArchetype(entity);
        }
        else
            entity = world.Entities.CreateEntityFromArchetype(m_Archetype, ComponentInitializationStrategy::Zero);

        auto data = world.Entities.GetEntityData(entity);
        size_t entitySize = world.Entities.GetEntityStorage(m_Archetype).GetEntitySize();

        if (data.has_value())
            std::memcpy(data.value(), m_Data, entitySize);

        return entity;
    }
}
