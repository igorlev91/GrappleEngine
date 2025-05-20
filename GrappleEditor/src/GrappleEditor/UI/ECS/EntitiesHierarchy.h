#pragma once

#include "GrappleCore/Core.h"

#include "GrappleECS/World.h"

namespace Grapple
{
	enum class EntitiesHierarchyFeatures
	{
		None = 0,
		CreateEntity = 1,
		DeleteEntity = 2,
		DuplicateEntity = 4,

		All = CreateEntity | DeleteEntity | DuplicateEntity,
	};
	Grapple_IMPL_ENUM_BITFIELD(EntitiesHierarchyFeatures);

	class EntitiesHierarchy
	{
	public:
		EntitiesHierarchy(EntitiesHierarchyFeatures features = EntitiesHierarchyFeatures::All);
		EntitiesHierarchy(World& world, EntitiesHierarchyFeatures features = EntitiesHierarchyFeatures::All);

		bool OnRenderImGui(Entity& selectedEntity);

		inline void SetWorld(World& world) { m_World = &world; }
	private:
		bool RenderEntityItem(Entity entity, Entity& selectedEntity);
		bool RenderEntityContextMenu(Entity entity, Entity& selectedEntity);
	private:
		EntitiesHierarchyFeatures m_Features;
		World* m_World;
	};
}