#include "PropertiesWindow.h"

#include "Grapple/Scene/Scene.h"
#include "Grapple/Scripting/ScriptingEngine.h"

#include "Grapple/Renderer/Material.h"

#include "GrappleEditor/UI/EditorGUI.h"
#include "GrappleEditor/UI/ECS/EntityProperties.h"

#include "GrappleEditor/EditorLayer.h"
#include "GrappleEditor/AssetManager/EditorShaderCache.h"
#include "GrappleEditor/AssetManager/MaterialImporter.h"

#include <imgui.h>

namespace Grapple
{
	TextureImportSettings s_SelectedTextureImportSettings;

	PropertiesWindow::PropertiesWindow(AssetManagerWindow& assetManagerWindow)
		: m_AssetManagerWindow(assetManagerWindow) {}

	void PropertiesWindow::OnAttach()
	{
		m_AssetManagerWindow.OnAssetSelectionChanged.Bind([](AssetHandle handle)
		{
			if (!AssetManager::IsAssetHandleValid(handle))
				return;

			switch (AssetManager::GetAssetMetadata(handle)->Type)
			{
			case AssetType::Texture:
				s_SelectedTextureImportSettings = TextureImportSettings();
				TextureImporter::DeserializeImportSettings(handle, s_SelectedTextureImportSettings);
				break;
			}
		});
	}

	void PropertiesWindow::OnImGuiRender()
	{
		ImGui::Begin("Properties");

		if (Scene::GetActive() == nullptr)
		{
			ImGui::End();
			return;
		}

		World& world = Scene::GetActive()->GetECSWorld();
		const EditorSelection& selection = EditorLayer::GetInstance().Selection;

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();

		if (selection.GetType() == EditorSelectionType::Entity)
		{
			Entity selectedEntity = selection.GetEntity();
			if (world.IsEntityAlive(selectedEntity))
			{
				EntityProperties properties(world);
				properties.OnRenderImGui(selectedEntity);
			}
		}
		else if (selection.GetType() == EditorSelectionType::Asset)
		{
			AssetHandle handle = selection.GetAssetHandle();
			if (AssetManager::IsAssetHandleValid(handle))
				RenderAssetProperties(handle);
		}

		ImGui::End();
	}

	void PropertiesWindow::RenderAssetProperties(AssetHandle handle)
	{
		Grapple_CORE_ASSERT(AssetManager::IsAssetHandleValid(handle));

		switch (AssetManager::GetAssetMetadata(handle)->Type)
		{
		case AssetType::Scene:
			break;
		case AssetType::Texture:
			RenderTextureImportSettingsEditor(handle, s_SelectedTextureImportSettings);
			break;
		case AssetType::Material:
			RenderMaterialEditor(handle);
			break;
		}
	}

	bool PropertiesWindow::RenderTextureImportSettingsEditor(AssetHandle handle, TextureImportSettings& importSettings)
	{
		Grapple_CORE_ASSERT(AssetManager::IsAssetHandleValid(handle));
		Grapple_CORE_ASSERT(AssetManager::GetAssetMetadata(handle)->Type == AssetType::Texture);

		if (ImGui::Button("Save"))
		{
			TextureImporter::SerialiazeImportSettings(handle, importSettings);
			As<EditorAssetManager>(AssetManager::GetInstance())->ReloadAsset(handle);
		}

		if (ImGui::BeginChild("Texture Import Settings"))
		{
			if (EditorGUI::BeginPropertyGrid())
			{
				const char* previewText = TextureFilteringToString(importSettings.Filtering);

				EditorGUI::PropertyName("Filtering");

				{
					ImGui::PushID(&importSettings.Filtering);
					if (ImGui::BeginCombo("", previewText, ImGuiComboFlags_HeightRegular))
					{
						bool selected = importSettings.Filtering == TextureFiltering::Linear;
						if (ImGui::Selectable(TextureFilteringToString(TextureFiltering::Linear), selected))
							importSettings.Filtering = TextureFiltering::Linear;

						if (selected)
							ImGui::SetItemDefaultFocus();

						selected = importSettings.Filtering == TextureFiltering::Closest;
						if (ImGui::Selectable(TextureFilteringToString(TextureFiltering::Closest), selected))
							importSettings.Filtering = TextureFiltering::Closest;

						if (selected)
							ImGui::SetItemDefaultFocus();

						ImGui::EndCombo();
					}
					ImGui::PopID();
				}

				{
					previewText = TextureWrapToString(importSettings.WrapMode);
					EditorGUI::PropertyName("Wrap");

					ImGui::PushID(&importSettings.WrapMode);
					if (ImGui::BeginCombo("", previewText, ImGuiComboFlags_HeightRegular))
					{
						bool selected = importSettings.WrapMode == TextureWrap::Repeat;
						if (ImGui::Selectable(TextureWrapToString(TextureWrap::Repeat), selected))
							importSettings.WrapMode = TextureWrap::Repeat;

						if (selected)
							ImGui::SetItemDefaultFocus();

						selected = importSettings.WrapMode == TextureWrap::Clamp;
						if (ImGui::Selectable(TextureWrapToString(TextureWrap::Clamp), selected))
							importSettings.WrapMode = TextureWrap::Clamp;

						if (selected)
							ImGui::SetItemDefaultFocus();

						ImGui::EndCombo();
					}
					ImGui::PopID();
				}

				EditorGUI::EndPropertyGrid();
			}

			ImGui::EndChild();
		}

		return false;
	}

	bool PropertiesWindow::RenderMaterialEditor(AssetHandle handle)
	{
		Grapple_CORE_ASSERT(AssetManager::IsAssetHandleValid(handle));
		Ref<Material> material = AssetManager::GetAsset<Material>(handle);

		const AssetMetadata* metadata = AssetManager::GetAssetMetadata(handle);

		if (ImGui::Button("Save"))
			MaterialImporter::SerializeMaterial(material, metadata->Path);

		AssetHandle shaderHandle = NULL_ASSET_HANDLE;
		bool hasShader = material->GetShader() != nullptr;

		if (hasShader)
			shaderHandle = material->GetShader()->Handle;

		if (EditorGUI::BeginPropertyGrid())
		{
			if (EditorGUI::AssetField("Shader", shaderHandle))
			{
				if (AssetManager::IsAssetHandleValid(shaderHandle))
				{
					if (AssetManager::GetAssetMetadata(shaderHandle)->Type == AssetType::Shader)
						material->SetShader(AssetManager::GetAsset<Shader>(shaderHandle));
					else
						Grapple_CORE_ERROR("Provided asset is not a shader");
				}
			}

			ImGui::BeginDisabled(true);

			if (hasShader)
			{
				ShaderFeatures features = material->GetShader()->GetFeatures();

				EditorGUI::BoolPropertyField("Depth Test", features.DepthTesting);
				EditorGUI::BoolPropertyField("Depth Write", features.DepthWrite);
				EditorGUI::PropertyName("Culling Mode");

				const char* preview = CullingModeToString(features.Culling);
			
				ImGui::PushID("CullingMode");
				if (ImGui::BeginCombo("", preview, ImGuiComboFlags_HeightRegular))
				{
					if (ImGui::MenuItem("None"))
						features.Culling = CullingMode::None;
					if (ImGui::MenuItem("Front"))
						features.Culling = CullingMode::Front;
					if (ImGui::MenuItem("Back"))
						features.Culling = CullingMode::Back;

					ImGui::EndCombo();
				}

				ImGui::PopID();

				// Blend Mode

				preview = BlendModeToString(features.Blending);

				EditorGUI::PropertyName("Blend Mode");

				ImGui::PushID("BlendMode");
				if (ImGui::BeginCombo("", preview, ImGuiComboFlags_HeightRegular))
				{
					if (ImGui::MenuItem("Opaque"))
						features.Blending = BlendMode::Opaque;
					if (ImGui::MenuItem("Transparent"))
						features.Blending = BlendMode::Transparent;

					ImGui::EndCombo();
				}

				ImGui::PopID();

				// Depth Comaprison Function

				preview = DepthComparisonFunctionToString(features.DepthFunction);

				EditorGUI::PropertyName("Depth Function");

				ImGui::PushID("Depth Function");
				if (ImGui::BeginCombo("", preview, ImGuiComboFlags_HeightRegular))
				{
					if (ImGui::MenuItem("Less"))
						features.DepthFunction = DepthComparisonFunction::Less;
					if (ImGui::MenuItem("Greater"))
						features.DepthFunction = DepthComparisonFunction::Greater;
					if (ImGui::MenuItem("Less or equal"))
						features.DepthFunction = DepthComparisonFunction::LessOrEqual;
					if (ImGui::MenuItem("Greater or equal"))
						features.DepthFunction = DepthComparisonFunction::GreaterOrEqual;
					if (ImGui::MenuItem("Equal"))
						features.DepthFunction = DepthComparisonFunction::Equal;
					if (ImGui::MenuItem("Not equal"))
						features.DepthFunction = DepthComparisonFunction::NotEqual;
					if (ImGui::MenuItem("Never"))
						features.DepthFunction = DepthComparisonFunction::Never;
					if (ImGui::MenuItem("Always"))
						features.DepthFunction = DepthComparisonFunction::Always;
				
					ImGui::EndCombo();
				}

				ImGui::PopID();
			}

			ImGui::EndDisabled();

			EditorGUI::EndPropertyGrid();
		}

		EditorShaderCache* shaderCache = (EditorShaderCache*)ShaderCacheManager::GetInstance().get();

		std::optional<const EditorShaderCache::ShaderEntry*> shaderEntry = shaderCache->GetShaderEntry(shaderHandle);
		if (shaderEntry && EditorGUI::BeginPropertyGrid())
		{
			const SerializableObjectDescriptor& serializationDescriptor = shaderEntry.value()->SerializationDescriptor;
			SerializableObject materialProperties = SerializableObject(material->GetPropertiesBuffer(), serializationDescriptor);

			EditorGUI::ObjectField(materialProperties);

			EditorGUI::EndPropertyGrid();
		}

		return false;
	}
}