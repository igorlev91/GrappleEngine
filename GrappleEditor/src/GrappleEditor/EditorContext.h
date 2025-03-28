#pragma once

#include "Grapple/Core/Core.h"
#include "Grapple/Scene/Scene.h"

#include "Grapple/AssetManager/Asset.h"

#include "GrappleECS/Entity/Entity.h"

namespace Grapple
{
	struct EditorContext
	{
	public:
		static void Initialize();
		static void OpenScene(AssetHandle handle);

		static const Ref<Scene>& GetActiveScene() { return Instance.m_ActiveScene; }
		static AssetHandle GetActiveSceneHandle() { return Instance.m_ActiveSceneHandle; }
	public:

		Entity SelectedEntity;
	private:
		Ref<Scene> m_ActiveScene;
		AssetHandle m_ActiveSceneHandle;
	public:
		static EditorContext Instance;
	};
}