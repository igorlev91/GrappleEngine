#pragma once

#include "Grapple/Renderer/RenderPass.h"
#include "Grapple/Renderer/Material.h"

namespace Grapple
{
	class Grapple_API ToneMapping : public RenderPass
	{
	public:
		Grapple_TYPE;

		ToneMapping();

		void OnRender(RenderingContext& context) override;
	public:
		bool Enabled;
	private:
		Ref<Material> m_Material;
	};
}