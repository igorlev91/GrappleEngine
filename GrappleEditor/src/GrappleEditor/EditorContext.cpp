#include "EditorContext.h"

#include "Grapple/AssetManager/AssetManager.h"
#include "GrappleEditor/AssetManager/EditorAssetManager.h"

namespace Grapple
{
	EditorContext EditorContext::Instance{};

	void EditorContext::Initialize()
	{
		Instance.m_ActiveScene = CreateRef<Scene>();
		Instance.m_ActiveSceneHandle = NULL_ASSET_HANDLE;
	}

	void EditorContext::OpenScene(AssetHandle handle)
	{
		if (AssetManager::IsAssetHandleValid(handle))
		{
			Ref<EditorAssetManager> editorAssetManager = As<EditorAssetManager>(AssetManager::GetInstance());
			editorAssetManager->UnloadAsset(Instance.m_ActiveSceneHandle);
			
			Instance.m_ActiveScene = AssetManager::GetAsset<Scene>(handle);
		}
	}
}