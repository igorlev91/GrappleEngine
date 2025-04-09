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

	public:
		Entity SelectedEntity;
		EditorMode Mode;
		GizmoMode Gizmo;
	public:
		static EditorContext Instance;
	};
}