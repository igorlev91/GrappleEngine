#pragma once

#include "Grapple/Scene/Components.h"

#include "GrappleEditor/AssetManager/TextureImporter.h"
#include "GrappleEditor/UI/AssetManagerWindow.h"

#include <unordered_map>
#include <functional>

namespace Grapple
{
	class PropertiesWindow
	{
	public:
		PropertiesWindow(AssetManagerWindow& assetManagerWindow);

		void OnAttach();
		void OnImGuiRender();
	private:
		void RenderAssetProperties(AssetHandle handle);

		bool RenderTextureSettingsEditor(AssetHandle handle, TextureImportSettings& importSettings);
		bool RenderMaterialEditor(AssetHandle handle);
	private:
		AssetManagerWindow& m_AssetManagerWindow;
	};
}