#pragma once

#include "Grapple/Scene/Prefab.h"
#include "Grapple/Scene/Scene.h"

#include "GrappleECS/World.h"

#include "GrappleEditor/UI/AssetEditor.h"
#include "GrappleEditor/UI/ECS/EntitiesHierarchy.h"
#include "GrappleEditor/UI/ECS/EntityProperties.h"

#include "GrappleEditor/UI/SceneViewportWindow.h"

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
		inline World& GetWorld() { return m_PreviewScene->GetECSWorld(); }
	private:
		Ref<Scene> m_PreviewScene;
		
		EditorCamera m_EditorCamera;
		SceneViewportWindow m_ViewportWindow;

		EntitiesHierarchy m_Entities;
		EntityProperties m_Properties;
		Ref<Prefab> m_Prefab;

		Entity m_SelectedEntity;
	};
}