#pragma once

#include "Grapple/AssetManager/Asset.h"

#include "GrappleECS/World.h"
#include "GrappleECS/Entity/Entity.h"

namespace Grapple
{
	class Grapple_API Prefab : public Asset
	{
	public:
		Prefab(std::vector<ComponentId>&& components, const uint8_t* prefabData)
			: Asset(AssetType::Prefab), m_Data(prefabData), m_Archetype(INVALID_ARCHETYPE_ID), m_Components(std::move(components)) {}
		Prefab(ArchetypeId archetype, const uint8_t* prefabData)
			: Asset(AssetType::Prefab), m_Data(prefabData), m_Archetype(archetype) {}

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
		ArchetypeId m_Archetype;
		std::vector<ComponentId> m_Components;
		const uint8_t* m_Data;
	};
}