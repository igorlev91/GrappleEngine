#pragma once

#include "Grapple/Core/Core.h"
#include "Grapple/Scene/Scene.h"

#include "GrappleECS/Entity/Entity.h"

namespace Grapple
{
	struct EditorContext
	{
	public:
		Ref<Scene> ActiveScene;
		Entity SelectedEntity;
		
		static EditorContext Instance;
	};
}