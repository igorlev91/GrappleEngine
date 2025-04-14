#pragma once

#include "Grapple/Core/Core.h"
#include "Grapple/Core/KeyCode.h"
#include "Grapple/Core/MouseCode.h"

#include "GrapplePlatform/Event.h"

#include <glm/glm.hpp>

namespace Grapple
{
	class Grapple_API InputManager
	{
	public:
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

		static bool IsKeyPressed(KeyCode key);
		static bool IsMouseButtonPreseed(MouseCode button);

		static void SetMousePositionOffset(const glm::ivec2& offset);
		static glm::ivec2 GetMousePositionOffset();

		static glm::ivec2 GetMousePosition();
		static glm::ivec2 GetRawMousePosition();

		static glm::ivec2 GetMouseDelta();
	};
}