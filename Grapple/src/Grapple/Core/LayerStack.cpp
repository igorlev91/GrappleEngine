#include "LayerStack.h"

namespace Grapple
{
	LayerStack::LayerStack()
	{
		m_LayerInsertPosition = 0;
	}

	void LayerStack::PushLayer(const Ref<Layer>& layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertPosition, layer);
		m_LayerInsertPosition++;
	}
	
	void LayerStack::PushOverlay(const Ref<Layer>& layer)
	{
		m_Layers.push_back(layer);
	}
}