#pragma once

#include "Grapple.h"
#include "Grapple/Core/Layer.h"
#include "Grapple/ImGui/ImGuiTheme.h"

namespace Grapple
{
	class ImGUILayer : public Layer
	{
	public:
		ImGUILayer();
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();
		void End();
	private:
		void SetThemeColors();
	};
}