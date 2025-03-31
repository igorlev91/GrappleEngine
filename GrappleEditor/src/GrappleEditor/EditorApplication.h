#pragma once

#include "Grapple/Core/Application.h"
#include "Grapple/Renderer2D/Renderer2D.h"

#include "Grapple/Core/CommandLineArguments.h"

namespace Grapple
{
	class EditorApplication : public Application
	{
	public:
		EditorApplication(CommandLineArguments arguments);
		~EditorApplication();
	};
}