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
	}

	void EditorContext::Uninitialize()
	{
		Instance.SelectedEntity = Entity();
	}
}