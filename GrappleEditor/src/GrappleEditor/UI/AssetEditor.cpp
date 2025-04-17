#include "AssetEditor.h"

namespace Grapple
{
	void AssetEditor::Open(AssetHandle handle)
	{
		m_Show = true;
		OnOpen(handle);
	}

	void AssetEditor::Close()
	{
		m_Show = false;
		OnClose();
	}

	void Grapple::AssetEditor::OnUpdate()
	{
		if (m_Show)
		{
			OnRenderImGui(m_Show);

			if (!m_Show)
				Close();
		}
	}
}
