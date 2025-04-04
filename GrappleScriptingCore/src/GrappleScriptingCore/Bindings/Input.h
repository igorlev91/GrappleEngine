#pragma once

#include "Grapple/Core/KeyCode.h"
#include "Grapple/Core/MouseCode.h"

namespace Grapple::Internal
{
	struct InputBindings
	{
		using IsKeyPressedFunction = bool(*)(KeyCode key);
		IsKeyPressedFunction IsKeyPressed;

		using IsMouseButtonPressedFunction = bool(*)(MouseCode button);
		IsMouseButtonPressedFunction IsMouseButtonPressed;

		static InputBindings Bindings;
	};

	class Input
	{
	public:
		inline static bool IsKeyPressed(KeyCode key) { return InputBindings::Bindings.IsKeyPressed(key); }
		inline static bool IsMouseButtonPressed(MouseCode button) { return InputBindings::Bindings.IsMouseButtonPressed(button); }
	};
}