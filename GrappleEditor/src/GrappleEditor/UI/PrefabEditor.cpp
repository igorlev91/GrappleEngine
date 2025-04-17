#include "PrefabEditor.h"

#include "Grapple/AssetManager/AssetManager.h"
#include "GrappleEditor/AssetManager/PrefabImporter.h"

#include <imgui.h>

namespace Grapple
{
	PrefabEditor::PrefabEditor(ECSContext& context)
		: m_Entities(m_PrefabWorld, EntitiesHierarchyFeatures::None), m_PrefabWorld(context), m_Properties(m_PrefabWorld), m_SelectedEntity(Entity())
	{
	}

	void PrefabEditor::OnOpen(AssetHandle asset)
	{
		Grapple_CORE_ASSERT(AssetManager::IsAssetHandleValid(asset));
		m_Prefab = AssetManager::GetAsset<Prefab>(asset);

		m_Prefab->CreateInstance(m_PrefabWorld);
	}

	void PrefabEditor::OnClose()
	{
		AssetHandle prefabHandle = m_Prefab->Handle;
		Entity entity = Entity();
		if (m_PrefabWorld.Entities.GetEntityRecords().size() > 0)
			entity = m_PrefabWorld.Entities.GetEntityRecords()[0].Id;

		PrefabImporter::SerializePrefab(prefabHandle, m_PrefabWorld, entity);

		m_Prefab = nullptr;
		m_PrefabWorld.Entities.Clear();
	}

	void PrefabEditor::OnRenderImGui(bool& show)
	{
		ImGui::Begin("Prefab Preview", &show);

		ImGui::End();

		ImGui::Begin("Prefab Entities");
		m_Entities.OnRenderImGui(m_SelectedEntity);
		ImGui::End();

		ImGui::Begin("Prefab Entity Properties");
		m_Properties.OnRenderImGui(m_SelectedEntity);
		ImGui::End();
	}
}