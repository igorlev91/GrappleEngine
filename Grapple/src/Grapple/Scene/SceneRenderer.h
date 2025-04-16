#pragma once

#include "Grapple/Renderer2D/Renderer2D.h"
#include "Grapple/AssetManager/AssetManager.h"
#include "Grapple/Scene/Components.h"

#include "GrappleECS/World.h"
#include "GrappleECS/System/SystemInitializer.h"

namespace Grapple
{
	struct SpritesRendererSystem : public System
	{
	public:
		SpritesRendererSystem();

		virtual void OnConfig(SystemConfig& config) override {}
		virtual void OnUpdate(SystemExecutionContext& context) override;
	private:
		Query m_SpritesQuery;
		std::vector<std::pair<Entity, int32_t>> m_SortedEntities;
	};
}