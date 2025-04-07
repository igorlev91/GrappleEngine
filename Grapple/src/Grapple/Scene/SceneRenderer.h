#pragma once

#include "Grapple/Renderer2D/Renderer2D.h"
#include "Grapple/AssetManager/AssetManager.h"
#include "Grapple/Scene/Components.h"

#include "GrappleECS/World.h"

namespace Grapple
{
	struct SpritesRendererSystem
	{
		static Query Setup(World& world);
		static void OnUpdate(SystemExecutionContext& context);
	};
}