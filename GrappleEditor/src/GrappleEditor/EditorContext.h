#pragma once

#include "Grapple/Core/Core.h"
#include "Grapple/Scene/Scene.h"

#include "Grapple/AssetManager/Asset.h"

#include "GrappleECS/Entity/Entity.h"

namespace Grapple
{
	enum class EditorMode
	{
		Edit,
		Play,
	};

	enum class GizmoMode
	{
		None,
		Translate,
		Rotate,
		Scale,
	};

	struct EditorContext
	{
	public:
		static void Initialize();
		static void Uninitialize();

		static void OpenScene(AssetHandle handle);

		static const Ref<Scene>& GetActiveScene() { return Instance.m_ActiveScene; }
		static const Ref<Scene>& GetEditedScene() { return Instance.m_EditedScene; }

		static void SetActiveScene(const Ref<Scene>& scene) { Instance.m_ActiveScene = scene; }
	public:
		Entity SelectedEntity;
		EditorMode Mode;
		GizmoMode Gizmo;
	private:
		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditedScene;
	public:
		static EditorContext Instance;
	};
}