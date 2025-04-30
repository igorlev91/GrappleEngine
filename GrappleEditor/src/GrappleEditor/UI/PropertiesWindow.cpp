#include "PropertiesWindow.h"

#include "Grapple/Scene/Scene.h"
#include "Grapple/Scripting/ScriptingEngine.h"

#include "Grapple/Renderer/Material.h"

#include "GrappleEditor/UI/EditorGUI.h"
#include "GrappleEditor/UI/ECS/EntityProperties.h"

#include "GrappleEditor/EditorLayer.h"
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

		if (EditorGUI::BeginPropertyGrid())
		{
			AssetHandle shaderHandle = material->GetShader()->Handle;
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

			EditorGUI::BoolPropertyField("Depth Test", material->Features.DepthTesting);
			EditorGUI::PropertyName("Culling Mode");

			const char* preview = CullingModeToString(material->Features.Culling);
			
			ImGui::PushID("CullingMode");
			if (ImGui::BeginCombo("", preview, ImGuiComboFlags_HeightRegular))
			{
				if (ImGui::MenuItem("None"))
					material->Features.Culling = CullingMode::None;
				if (ImGui::MenuItem("Front"))
					material->Features.Culling = CullingMode::Front;
				if (ImGui::MenuItem("Back"))
					material->Features.Culling = CullingMode::Back;

				ImGui::EndCombo();
			}

			ImGui::PopID();

			EditorGUI::EndPropertyGrid();
		}

		if (EditorGUI::BeginPropertyGrid())
		{
			Ref<Shader> shader = material->GetShader();
			const ShaderParameters& shaderParameters = shader->GetParameters();
			for (uint32_t i = 0; i < (uint32_t)shaderParameters.size(); i++)
			{
				switch (shaderParameters[i].Type)
				{
				case ShaderDataType::Int:
				{
					int32_t value = material->ReadParameterValue<int32_t>(i);
					if (EditorGUI::IntPropertyField(shaderParameters[i].Name.c_str(), value))
						material->WriteParameterValue(i, value);

					break;
				}
				case ShaderDataType::Float:
				{
					float value = material->ReadParameterValue<float>(i);
					if (EditorGUI::FloatPropertyField(shaderParameters[i].Name.c_str(), value))
						material->WriteParameterValue(i, value);

					break;
				}
				case ShaderDataType::Float2:
				{
					glm::vec2 value = material->ReadParameterValue<glm::vec2>(i);
					if (EditorGUI::Vector2PropertyField(shaderParameters[i].Name.c_str(), value))
						material->WriteParameterValue(i, value);

					break;
				}
				case ShaderDataType::Float3:
				{
					glm::vec3 value = material->ReadParameterValue<glm::vec3>(i);
					if (EditorGUI::Vector3PropertyField(shaderParameters[i].Name.c_str(), value))
						material->WriteParameterValue(i, value);

					break;
				}
				case ShaderDataType::Float4:
				{
					glm::vec4 value = material->ReadParameterValue<glm::vec4>(i);
					if (EditorGUI::Vector4PropertyField(shaderParameters[i].Name.c_str(), value))
						material->WriteParameterValue(i, value);

					break;
				}
				default:
					Grapple_CORE_ASSERT("Unhandled shader data type");
				}
			}

			EditorGUI::EndPropertyGrid();
		}

		return false;
	}
}