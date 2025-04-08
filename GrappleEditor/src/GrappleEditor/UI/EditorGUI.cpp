#include "EditorGUI.h"

#include "Grapple/AssetManager/AssetManager.h"
#include "GrappleEditor/AssetManager/EditorAssetManager.h"

#include <spdlog/spdlog.h>

#include <imgui.h>

namespace Grapple
{
	bool EditorGUI::BeginPropertyGrid()
	{
		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		if (ImGui::BeginTable("Property Grid", 2))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, windowSize.x * 0.25f);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, windowSize.x * 0.75f);
			return true;
		}
		return false;
	}

	void EditorGUI::EndPropertyGrid()
	{
		ImGui::EndTable();
	}

	bool EditorGUI::BeginMenu(const char* name)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		if (ImGui::Button(name))
		{
			ImVec2 buttonMin = ImGui::GetItemRectMin();
			ImVec2 buttonMax = ImGui::GetItemRectMax();
			ImGui::SetNextWindowPos(ImVec2(buttonMin.x, buttonMax.y));
			ImGui::OpenPopup(name);
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(1);
		
		return ImGui::BeginPopup(name);
	}

	void EditorGUI::EndMenu()
	{
		ImGui::EndMenu();
	}

	bool EditorGUI::FloatPropertyField(const char* name, float& value)
	{
		RenderPropertyName(name);

		ImGui::PushID(&value);
		bool result = ImGui::DragFloat("", &value, 0.1f);
		ImGui::PopID();

		ImGui::PopItemWidth();
		return result;
	}

	bool EditorGUI::Vector2PropertyField(const char* name, glm::vec2& value)
	{
		RenderPropertyName(name);

		ImGui::PushID(&value);
		bool result = ImGui::DragFloat2("", glm::value_ptr(value), 0.1f);
		ImGui::PopID();
		return result;
	}

	bool EditorGUI::Vector3PropertyField(const char* name, glm::vec3& value)
	{
		RenderPropertyName(name);

		ImGui::PushID(&value);
		bool result = ImGui::DragFloat3("", glm::value_ptr(value), 0.1f);
		ImGui::PopID();
		return result;
	}
	bool EditorGUI::ColorPropertyField(const char* name, glm::vec4& color)
	{
		RenderPropertyName(name);

		ImGui::PushID(&color);
		bool result = ImGui::ColorEdit4("", glm::value_ptr(color), ImGuiColorEditFlags_Float);
		ImGui::PopID();
		return result;
	}

	bool EditorGUI::AssetField(const char* name, AssetHandle& handle)
	{
		RenderPropertyName(name);

		ImVec2 buttonSize = ImVec2(ImGui::GetContentRegionAvail().x - 60.0f, 0);

		if (handle == NULL_ASSET_HANDLE)
			ImGui::Button("None", buttonSize);
		else
		{
			const AssetMetadata* metadata = AssetManager::GetAssetMetadata(handle);

			if (metadata != nullptr)
				ImGui::Button(metadata->Path.filename().string().c_str(), buttonSize);
			else
				ImGui::Button("Invalid", buttonSize);
		}

		bool result = false;
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ASSET_PAYLOAD_NAME))
			{
				handle = *(AssetHandle*)payload->Data;
				result = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();

		if (ImGui::Button("Reset", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
		{
			handle = NULL_ASSET_HANDLE;
			return true;
		}

		return result;
	}

	bool EditorGUI::EntityField(const char* name, const World& world, Entity& entity)
	{
		bool alive = world.IsEntityAlive(entity);

		RenderPropertyName(name);

		float buttonWidth = ImGui::GetContentRegionAvail().x - 60.0f;

		if (alive)
			ImGui::Button(fmt::format("Enitty {0} {1}", entity.GetIndex(), entity.GetGeneration()).c_str(), ImVec2(buttonWidth, 0));
		else
			ImGui::Button("None", ImVec2(buttonWidth, 0));

		bool result = false;
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ENTITY_PAYLOAD_NAME))
			{
				entity = *(Entity*)payload->Data;
				result = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();

		if (ImGui::Button("Reset", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
		{
			entity = Entity();
			return true;
		}

		return result;
	}

	bool EditorGUI::BeginToggleGroup(const char* name)
	{
		RenderPropertyName(name);

		ImGui::NewLine();
		return true;
	}

	bool EditorGUI::ToggleGroupItem(const char* text, bool selected)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		if (selected)
			ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_FrameBg]);
		else
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

		ImGui::SameLine();
		bool result = ImGui::Button(text);

		if (selected)
			ImGui::PopStyleColor();
		else
			ImGui::PopStyleVar();

		return result;
	}

	void EditorGUI::EndToggleGroup()
	{
	}

	void EditorGUI::RenderPropertyName(const char* name)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		ImGui::Text(name);

		ImGui::TableSetColumnIndex(1);
		ImGui::PushItemWidth(-1);
	}
}