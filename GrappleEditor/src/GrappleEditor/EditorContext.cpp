#include "EditorContext.h"

#include "Grapple/AssetManager/AssetManager.h"
#include "Grapple/Scripting/ScriptingEngine.h"
#include "GrappleEditor/AssetManager/EditorAssetManager.h"

namespace Grapple
{
	EditorContext EditorContext::Instance{};

	void EditorContext::Initialize()
	{
		Instance.Gizmo = GizmoMode::Translate;

		Instance.m_ActiveScene = CreateRef<Scene>();
		Instance.m_EditedScene = Instance.m_ActiveScene;

		ScriptingEngine::SetCurrentECSWorld(Instance.m_ActiveScene->GetECSWorld());
		ScriptingEngine::RegisterComponents();
	}

	void EditorContext::Uninitialize()
	{
		Instance.m_ActiveScene = nullptr;
		Instance.m_EditedScene = nullptr;

		Instance.SelectedEntity = Entity();
	}

	void EditorContext::OpenScene(AssetHandle handle)
	{
		Grapple_CORE_ASSERT(Instance.Mode == EditorMode::Edit);

		if (AssetManager::IsAssetHandleValid(handle))
		{
			ScriptingEngine::UnloadAllModules();

			Ref<EditorAssetManager> editorAssetManager = As<EditorAssetManager>(AssetManager::GetInstance());

			if (AssetManager::IsAssetHandleValid(Instance.m_ActiveScene->Handle))
				editorAssetManager->UnloadAsset(Instance.m_ActiveScene->Handle);
			
			ScriptingEngine::LoadModules();

			Instance.m_ActiveScene = AssetManager::GetAsset<Scene>(handle);
			Instance.m_EditedScene = Instance.m_ActiveScene;

			ScriptingEngine::SetCurrentECSWorld(Instance.m_ActiveScene->GetECSWorld());
			ScriptingEngine::RegisterComponents();
		}
	}
}