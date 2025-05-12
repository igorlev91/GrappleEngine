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
		virtual void OnConfig(World& world, SystemConfig& config) override;
		virtual void OnUpdate(World& world, SystemExecutionContext& context) override;
	private:
		void RenderQuads(World& world, SystemExecutionContext& context);
		void RenderText(SystemExecutionContext& context);
	private:
		struct EntityQueueElement
		{
			Entity Id;
			int32_t SortingLayer;
			AssetHandle Material;
		};

		Query m_SpritesQuery;
		Query m_TextQuery;
		std::vector<EntityQueueElement> m_SortedEntities;
	};

	struct MeshesRendererSystem : public System
	{
	public:
		void OnConfig(World& world, SystemConfig& config) override;
		void OnUpdate(World& world, SystemExecutionContext& context) override;
	private:
		Query m_Query;
	};
}