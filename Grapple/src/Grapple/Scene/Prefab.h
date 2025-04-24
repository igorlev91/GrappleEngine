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
		Prefab(const uint8_t* prefabData, std::vector<std::pair<ComponentId, void*>>&& components)
			: Asset(AssetType::Prefab), m_Data(prefabData), m_Components(std::move(components)) {}
		
		~Prefab()
		{
			if (m_Data != nullptr)
			{
				delete[] m_Data;
				m_Data = nullptr;
			}
		}
	
		Entity CreateInstance(World& world);
	private:
		std::vector<std::pair<ComponentId, void*>> m_Components;
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