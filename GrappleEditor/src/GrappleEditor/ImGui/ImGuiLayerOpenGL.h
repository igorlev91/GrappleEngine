#pragma once

#include "GrappleEditor/ImGui/ImGuiLayer.h"

namespace Grapple
{
	class ImGuiLayerOpenGL : public ImGuiLayer
	{
	public:
		void InitializeRenderer() override;
		void ShutdownRenderer() override;

		void Begin() override;
		void End() override;
	};
}
