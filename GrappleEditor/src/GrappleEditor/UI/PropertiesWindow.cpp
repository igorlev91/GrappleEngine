#include "PropertiesWindow.h"

#include "Grapple/Scene/Scene.h"
#include "Grapple/Scripting/ScriptingEngine.h"

#include "Grapple/Renderer/ComputeShader.h"
#include "Grapple/Renderer/Material.h"
#include "Grapple/Renderer/MaterialsTable.h"

#include "GrappleEditor/UI/EditorGUI.h"
#include "GrappleEditor/UI/ECS/EntityProperties.h"
#include "GrappleEditor/UI/SpriteEditor.h"
#include "GrappleEditor/UI/SerializablePropertyRenderer.h"

#include "GrappleEditor/EditorLayer.h"
#include "GrappleEditor/AssetManager/EditorShaderCache.h"
#include "GrappleEditor/AssetManager/MaterialImporter.h"
#include "GrappleEditor/AssetManager/SpriteImporter.h"

#include <imgui.h>

namespace Grapple
{
	TextureImportSettings s_SelectedTextureImportSettings;

	PropertiesWindow::PropertiesWindow(AssetManagerWindow& assetManagerWindow)
		: m_AssetManagerWindow(assetManagerWindow) {}

	void PropertiesWindow::OnAttach()
	{
		m_AssetManagerWindow.OnAssetSelectionChanged.Bind([this](AssetHandle handle)
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
		bool windowVisible = ImGui::Begin("Properties");

		if (Scene::GetActive() == nullptr)
		{
			ImGui::End();
			return;
		}

		if (windowVisible)
		{
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
		}

		ImGui::End();
	}

	static void RenderTableRow(const char* rowName, int32_t value)
	{
		const ImGuiStyle& style = ImGui::GetStyle();

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		EditorGUI::MoveCursor(ImVec2(0, style.FramePadding.y));
		ImGui::TextUnformatted(rowName);

		ImGui::TableSetColumnIndex(1);

		EditorGUI::MoveCursor(ImVec2(0, style.FramePadding.y));
		ImGui::Text("%d", value);
	}

	static void RenderTableRow(const char* rowName, const char* text)
	{
		const ImGuiStyle& style = ImGui::GetStyle();

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		EditorGUI::MoveCursor(ImVec2(0, style.FramePadding.y));
		ImGui::TextUnformatted(rowName);

		ImGui::TableSetColumnIndex(1);

		EditorGUI::MoveCursor(ImVec2(0, style.FramePadding.y));
		ImGui::TextUnformatted(text);
	}

	void PropertiesWindow::RenderAssetProperties(AssetHandle handle)
	{
		Grapple_CORE_ASSERT(AssetManager::IsAssetHandleValid(handle));
		const AssetMetadata* assetMetadata = AssetManager::GetAssetMetadata(handle);
		Grapple_CORE_ASSERT(assetMetadata);

		switch (assetMetadata->Type)
		{
		case AssetType::Scene:
			break;
		case AssetType::Texture:
			RenderTextureSettingsEditor(handle, s_SelectedTextureImportSettings);
			break;
		case AssetType::Material:
			RenderMaterialEditor(handle);
			break;
		case AssetType::Mesh:
		{
			Ref<const Mesh> mesh = AssetManager::GetAsset<const Mesh>(handle);

			if (ImGui::BeginTable("MeshInfo", 2))
			{
				ImVec2 windowSize = ImGui::GetContentRegionAvail();
				ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, windowSize.x / 2.0f);
				ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, windowSize.x / 2.0f);

				RenderTableRow("Submeshes", (int32_t)mesh->GetSubMeshes().size());
				RenderTableRow("Total Vertices", (int32_t)mesh->GetVertexBufferSize());
				RenderTableRow("Total Indices", (int32_t)mesh->GetIndexBufferSize());

				const char* indexFormatText = "";
				switch (mesh->GetIndexFormat())
				{
				case IndexBuffer::IndexFormat::UInt16:
					indexFormatText = "UInt16";
					break;
				case IndexBuffer::IndexFormat::UInt32:
					indexFormatText = "UInt32";
					break;
				}

				RenderTableRow("Index format", indexFormatText);

				ImGui::EndTable();
			}
			break;
		}
		case AssetType::MaterialsTable:
		{
			Ref<MaterialsTable> table = AssetManager::GetAsset<MaterialsTable>(handle);
			Grapple_CORE_ASSERT(table);

			if (EditorGUI::BeginPropertyGrid())
			{
				for (size_t i = 0; i < table->Materials.size(); i++)
				{
					EditorGUI::PropertyIndex(i);
					EditorGUI::AssetField(table->Materials[i]);
				}

				EditorGUI::EndPropertyGrid();
			}

			break;
		}
		case AssetType::ComputeShader:
		{
			Ref<ComputeShader> computeShader = AssetManager::GetAsset<ComputeShader>(handle);
			if (EditorGUI::BeginPropertyGrid())
			{
				const ImGuiStyle& style = ImGui::GetStyle();
				EditorGUI::PropertyName("Name");

				EditorGUI::MoveCursor(ImVec2(0, style.FramePadding.y));
				ImGui::TextUnformatted(computeShader->GetMetadata()->Name.c_str());
				EditorGUI::MoveCursor(ImVec2(0, style.FramePadding.y));

				EditorGUI::PropertyName("Local Group Size");

				EditorGUI::MoveCursor(ImVec2(0, style.FramePadding.y));
				glm::uvec3 localGroupSize = computeShader->GetMetadata()->LocalGroupSize;
				ImGui::Text("X: %u Y: %u Z: %u", localGroupSize.x, localGroupSize.y, localGroupSize.z);
				EditorGUI::MoveCursor(ImVec2(0, style.FramePadding.y));
				EditorGUI::EndPropertyGrid();
			}
			break;
		}
		case AssetType::Sprite:
		{
			Ref<Sprite> sprite = AssetManager::GetAsset<Sprite>(handle);
			Grapple_CORE_ASSERT(sprite);

			if (ImGui::Button("Save"))
			{
				SpriteImporter::SerializeSprite(sprite, AssetManager::GetAssetMetadata(handle)->Path);
			}

			SerializablePropertyRenderer propertiesRenderer;
			propertiesRenderer.Serialize("Sprite", SerializationValue(*sprite));

			break;
		}
		}
	}

	bool PropertiesWindow::RenderTextureSettingsEditor(AssetHandle handle, TextureImportSettings& importSettings)
	{
		Grapple_CORE_ASSERT(AssetManager::IsAssetHandleValid(handle));
		Grapple_CORE_ASSERT(AssetManager::GetAssetMetadata(handle)->Type == AssetType::Texture);

		if (ImGui::Button("Save"))
		{
			TextureImporter::SerialiazeImportSettings(handle, importSettings);
			As<EditorAssetManager>(AssetManager::GetInstance())->ReloadAsset(handle);
		}

		Ref<const Texture> texture = AssetManager::GetAsset<Texture>(handle);
		Grapple_CORE_ASSERT(texture);

		if (ImGui::BeginChild("Texture Settings"))
		{
			const ImGuiStyle& style = ImGui::GetStyle();
			if (EditorGUI::BeginPropertyGrid())
			{
				EditorGUI::PropertyName("Size");

				EditorGUI::MoveCursor(ImVec2(0, style.FramePadding.y));
				ImGui::Text("%u x %u", texture->GetWidth(), texture->GetHeight());
				EditorGUI::MoveCursor(ImVec2(0, style.FramePadding.y));

				EditorGUI::PropertyName("Format");

				EditorGUI::MoveCursor(ImVec2(0, style.FramePadding.y));
				ImGui::TextUnformatted(TextureFormatToString(texture->GetSpecifications().Format));
				EditorGUI::MoveCursor(ImVec2(0, style.FramePadding.y));

				{
					const char* previewText = TextureFilteringToString(importSettings.Filtering);

					EditorGUI::PropertyName("Filtering");
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
					const char* previewText = TextureWrapToString(importSettings.WrapMode);
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

				EditorGUI::BoolPropertyField("Generate Mip Maps", importSettings.GenerateMipMaps);

				EditorGUI::EndPropertyGrid();
			}

			// Preview

			float scrollbarWidth = ImGui::GetCurrentWindow()->ScrollbarSizes.x;

			ImVec2 availiableContentSize = ImGui::GetContentRegionAvail() - ImVec2(scrollbarWidth, 0.0f);
			float width = (float)texture->GetWidth();
			float height = (float)texture->GetHeight();

			float aspectRatio = width / height;

			ImGui::Image(
				ImGuiLayer::GetId(texture),
				ImVec2(availiableContentSize.x, availiableContentSize.x / aspectRatio),
				ImVec2(0, 1), ImVec2(1, 0), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), style.Colors[ImGuiCol_Border]);

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
					{
						Ref<Shader> newShader = AssetManager::GetAsset<Shader>(shaderHandle);
						hasShader = newShader != nullptr;
						material->SetShader(newShader);
					}
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

		Ref<const ShaderMetadata> shaderEntry = shaderCache->FindShaderMetadata(shaderHandle);
		if (shaderEntry)
		{
			const ShaderProperties& properties = shaderEntry->Properties;

			SerializablePropertyRenderer propertyRenderer;
			for (size_t propertyIndex = 0; propertyIndex < properties.size(); propertyIndex++)
			{
				const ShaderProperty& property = properties[propertyIndex];
				if (property.Hidden)
					continue;

				propertyRenderer.PropertyKey(property.DisplayName);
				switch (property.Type)
				{
				case ShaderDataType::Int:
					propertyRenderer.Serialize(SerializationValue(material->GetPropertyValue<int32_t>((uint32_t)propertyIndex), property.Flags));
					break;
				case ShaderDataType::Int2:
					propertyRenderer.Serialize(SerializationValue(material->GetPropertyValue<glm::ivec2>((uint32_t)propertyIndex), property.Flags));
					break;
				case ShaderDataType::Int3:
					propertyRenderer.Serialize(SerializationValue(material->GetPropertyValue<glm::ivec3>((uint32_t)propertyIndex), property.Flags));
					break;
				case ShaderDataType::Int4:
					propertyRenderer.Serialize(SerializationValue(material->GetPropertyValue<glm::ivec4>((uint32_t)propertyIndex), property.Flags));
					break;

				case ShaderDataType::Float:
					propertyRenderer.Serialize(SerializationValue(material->GetPropertyValue<float>((uint32_t)propertyIndex), property.Flags));
					break;
				case ShaderDataType::Float2:
					propertyRenderer.Serialize(SerializationValue(material->GetPropertyValue<glm::vec2>((uint32_t)propertyIndex), property.Flags));
					break;
				case ShaderDataType::Float3:
					propertyRenderer.Serialize(SerializationValue(material->GetPropertyValue<glm::vec3>((uint32_t)propertyIndex), property.Flags));
					break;
				case ShaderDataType::Float4:
					propertyRenderer.Serialize(SerializationValue(material->GetPropertyValue<glm::vec4>((uint32_t)propertyIndex), property.Flags));
					break;

				case ShaderDataType::Sampler:
				{
					Ref<Texture> texture = material->GetTextureProperty((uint32_t)propertyIndex);
					propertyRenderer.Serialize(SerializationValue(texture, property.Flags));

					// TODO: Serialization should return something to identify that the value has changed
					material->SetTextureProperty((uint32_t)propertyIndex, texture);
					break;
				}
				}
			}

			if (propertyRenderer.PropertiesGridStarted())
				EditorGUI::EndPropertyGrid();
		}

		return false;
	}
}