#pragma once

#include "GrappleECS/Entity/Entity.h"
#include "GrappleECS/Entity/Archetype.h"

namespace Grapple
{
	class ECSInspector
	{
	public:
		void OnImGuiRender();

		static void Show();
		static ECSInspector& GetInstance();
	private:
		void RenderEntityInfo(Entity entity);
		void RenderArchetypeInfo(ArchetypeId archetype);
		void RenderSystem(uint32_t systemIndex);
	private:
		bool m_Shown;
	};
}