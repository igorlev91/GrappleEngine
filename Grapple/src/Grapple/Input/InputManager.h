#pragma once

#include "GrappleCore/Core.h"
#include "GrappleCore/KeyCode.h"
#include "GrappleCore/MouseCode.h"

#include "GrapplePlatform/Event.h"
#include "GrapplePlatform/Window.h"

#include <glm/glm.hpp>

namespace Grapple
{
	class Grapple_API InputManager
	{
	public:
		static void Initialize();
		static void Update();

		static void ProcessEvent(Event& event);

		static bool IsKeyHeld(KeyCode key);
		static bool IsKeyPressed(KeyCode key);
		static bool IsKeyReleased(KeyCode key);

		static bool IsMouseButtonHeld(MouseCode button);
		static bool IsMouseButtonPressed(MouseCode button);
		static bool IsMouseButtonReleased(MouseCode button);

		static glm::vec2 GetMouseScroll();

		static void SetMousePositionOffset(const glm::ivec2& offset);
		static glm::ivec2 GetMousePositionOffset();

		static glm::ivec2 GetMousePosition();
		static glm::ivec2 GetRawMousePosition();

		static glm::ivec2 GetMouseDelta();
		static void SetCursorMode(CursorMode mode);
		static CursorMode GetCursorMode();
	};
}