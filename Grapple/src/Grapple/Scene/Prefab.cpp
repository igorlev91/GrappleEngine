#include "Prefab.h"

#include "Grapple/AssetManager/AssetManager.h"

#include <yaml-cpp/yaml.h>

namespace Grapple
{
    Grapple_IMPL_ASSET(Prefab);
    Grapple_SERIALIZABLE_IMPL(Prefab);

    Entity Prefab::CreateInstance(World& world)
    {
        return world.Entities.CreateEntity(m_Components.data(), m_Components.size(), true);
    }

    InstantiatePrefab::InstantiatePrefab(const Ref<Prefab>& prefab)
        : m_Prefab(prefab) {}

    void InstantiatePrefab::Apply(CommandContext& context, World& world)
    {
        Entity entity = m_Prefab->CreateInstance(world);
        context.SetEntity(m_OutputEntity, entity);
    }

    void InstantiatePrefab::Initialize(FutureEntity entity)
    {
        m_OutputEntity = entity;
    }
}
