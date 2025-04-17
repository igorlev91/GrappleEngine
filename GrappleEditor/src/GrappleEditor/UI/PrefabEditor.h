#pragma once

#include "Grapple/Scene/Prefab.h"

#include "GrappleECS/World.h"

#include "GrappleEditor/UI/AssetEditor.h"
#include "GrappleEditor/UI/ECS/EntitiesHierarchy.h"
#include "GrappleEditor/UI/ECS/EntityProperties.h"

namespace Grapple
{
	class PrefabEditor : public AssetEditor
	{
	public:
		PrefabEditor(ECSContext& context);
	protected:
		virtual void OnOpen(AssetHandle asset) override;
		virtual void OnClose() override;
		virtual void OnRenderImGui(bool& show) override;
	private:
		World m_PrefabWorld;
		EntitiesHierarchy m_Entities;
		EntityProperties m_Properties;
		Ref<Prefab> m_Prefab;

		Entity m_SelectedEntity;
	};
}