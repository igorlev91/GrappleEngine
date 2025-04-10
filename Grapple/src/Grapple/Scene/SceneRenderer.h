#pragma once

#include "Grapple/Renderer2D/Renderer2D.h"
#include "Grapple/AssetManager/AssetManager.h"
#include "Grapple/Scene/Components.h"

#include "GrappleECS/World.h"

namespace Grapple
{
	struct SpritesRendererSystem : public System
	{
	public:
		SpritesRendererSystem();

		virtual void OnUpdate(SystemExecutionContext& context) override;
	private:
		Query m_SpritesQuery;
	};
}