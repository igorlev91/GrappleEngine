#pragma once

namespace Grapple
{
	class EditorGUI
	{
	public:
		static bool BeginPropertyGrid();
		static void EndPropertyGrid();

		static bool FloatPropertyField(const char* name, float& value);
	};
}