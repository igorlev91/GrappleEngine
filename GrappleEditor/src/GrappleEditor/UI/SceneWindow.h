#pragma once

#include "GrappleEditor/UI/ECS/EntitiesHierarchy.h"

namespace Grapple
{
	class SceneWindow
	{
	public:
		void OnImGuiRender();
	private:
		EntitiesHierarchy m_Hierarchy;
	};
}