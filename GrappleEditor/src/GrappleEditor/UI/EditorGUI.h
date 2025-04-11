#pragma once

#include "Grapple.h"

#include "Grapple/Scripting/ScriptingEngine.h"

#include "GrappleECS/EntityId.h"
#include "GrappleECS/World.h"

namespace Grapple
{
	constexpr char* ENTITY_PAYLOAD_NAME = "ENTITY_PAYLOAD";

	class EditorGUI
	{
	public:
		static bool BeginPropertyGrid();
		static void EndPropertyGrid();

		static bool BeginMenu(const char* name);
		static void EndMenu();

		static bool BoolPropertyField(const char* name, bool& value);
		static bool FloatPropertyField(const char* name, float& value);
		static bool Vector2PropertyField(const char* name, glm::vec2& value);
		static bool Vector3PropertyField(const char* name, glm::vec3& value);
		static bool ColorPropertyField(const char* name, glm::vec4& color);

		static bool AssetField(const char* name, AssetHandle& handle);
		static bool EntityField(const char* name, const World& world, Entity& entity);

		static bool BeginToggleGroup(const char* name, uint32_t itemsCount);
		static bool BeginToggleGroupProperty(const char* name, uint32_t itemsCount);
		static bool ToggleGroupItem(const char* text, bool selected);
		static void EndToggleGroup();

		static bool TypeEditor(const Scripting::ScriptingType& type, uint8_t* data);
	private:
		static void RenderPropertyName(const char* name);
	};
}