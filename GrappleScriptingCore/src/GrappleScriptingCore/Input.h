#pragma once

#include "Grapple/Core/KeyCode.h"
#include "Grapple/Core/MouseCode.h"

#include "GrappleScriptingCore/Bindings.h"

namespace Grapple::Scripting
{
	class Input
	{
	public:
		inline static bool IsKeyPressed(KeyCode key)
		{
			return Bindings::Instance->IsKeyPressed(key);
		}

		inline static bool IsMouseButtonPressed(MouseCode button)
		{
			return Bindings::Instance->IsMouseButtonPressed(button);
		}
	};
}