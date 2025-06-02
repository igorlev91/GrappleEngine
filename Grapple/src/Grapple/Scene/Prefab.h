#pragma once

#include "Grapple/AssetManager/Asset.h"

#include "GrappleECS/World.h"
#include "GrappleECS/Entity/Entity.h"
#include "GrappleECS/Commands/Command.h"

namespace Grapple
{
	class Grapple_API Prefab : public Asset
	{
	public:
		Grapple_ASSET;
		Grapple_SERIALIZABLE;

		Prefab(const uint8_t* prefabData, const Components* compatibleComponentsRegistry, std::vector<std::pair<ComponentId, void*>>&& components);	
		~Prefab();
	
		Entity CreateInstance(World& world);
	private:
		std::vector<std::pair<ComponentId, void*>> m_Components;
		const Components* m_CompatibleComponentsRegistry = nullptr;
		const uint8_t* m_Data;
	};
	
	class Grapple_API InstantiatePrefab : public EntityCommand
	{
	public:
		InstantiatePrefab() = default;
		InstantiatePrefab(const Ref<Prefab>& prefab);

		virtual void Apply(CommandContext& context, World& world) override;
		virtual void Initialize(FutureEntity entity);
	private:
		FutureEntity m_OutputEntity;
		Ref<Prefab> m_Prefab;
	};
}