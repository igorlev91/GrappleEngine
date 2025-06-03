#pragma once

#include "Grapple/AssetManager/Asset.h"

namespace Grapple
{
	class ShaderLibraryWindow
	{
	public:
		void OnRenderImGui();

		static void Show();
		static ShaderLibraryWindow& GetInstance();
	private:
		void RenderShaderItem(AssetHandle handle);
	private:
		bool m_Show = false;
	};
}
