#pragma once

#include "Grapple.h"
#include "Grapple/Core/Layer.h"

#include <vector>

namespace Grapple
{
	class Grapple_API LayerStack
	{
	public:
		LayerStack();
	public:
		void PushLayer(const Ref<Layer>& layer);
		void PushOverlay(const Ref<Layer>& layer);

		std::vector<Ref<Layer>>& GetLayers() { return m_Layers; }
		const std::vector<Ref<Layer>>& GetLayers() const { return m_Layers; }
	private:
		std::vector<Ref<Layer>> m_Layers;
		size_t m_LayerInsertPosition;
	};
}