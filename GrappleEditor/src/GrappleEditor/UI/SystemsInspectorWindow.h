#pragma once

namespace Grapple
{
	class SystemsInspectorWindow
	{
	public:
		static void OnImGuiRender();
		static void Show();
	private:
		static bool s_Opened;
	};
}