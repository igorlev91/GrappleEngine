#pragma once

#include "Grapple/Core/KeyCode.h"
#include "Grapple/Core/MouseCode.h"
#include "Grapple/Core/Event.h"

#include <glm/glm.hpp>

namespace Grapple
{
	class InputManager
	{
	private:
		struct Data
		{
			glm::ivec2 PreviousMousePosition;
			glm::ivec2 MousePosition;

			glm::ivec2 MousePositionOffset;

			bool MouseButtonsState[(size_t)MouseCode::ButtonLast + 1];
			bool KeyState[(size_t)KeyCode::Menu + 1];
		};
	public:
		static void Initialize();
		static void ProcessEvent(Event& event);

		static bool IsKeyPressed(KeyCode key) { return s_Data.KeyState[(size_t)key]; }
		static bool IsMouseButtonPreseed(MouseCode button) { return s_Data.MouseButtonsState[(size_t)button]; }

		static void SetMousePositionOffset(const glm::ivec2& offset) { s_Data.MousePositionOffset = offset; }
		static glm::ivec2 GetMousePositionOffset() { return s_Data.MousePositionOffset; }

		static glm::ivec2 GetMousePosition() { return s_Data.MousePosition + s_Data.MousePositionOffset; }
		static glm::ivec2 GetRawMousePosition() { return s_Data.MousePosition; }

		static glm::ivec2 GetMouseDelta() { return s_Data.MousePosition - s_Data.PreviousMousePosition; }
	private:
		static Data s_Data;
	};
}