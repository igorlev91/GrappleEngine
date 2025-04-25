#pragma once

#include "Grapple/Renderer/RenderPass.h"
#include "Grapple/Renderer/Shader.h"

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
		Ref<Shader> m_Shader;
	};
}