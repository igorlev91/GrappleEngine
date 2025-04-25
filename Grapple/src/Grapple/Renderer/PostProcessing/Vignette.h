#pragma once

#include "Grapple/Renderer/RenderPass.h"
#include "Grapple/Renderer/Shader.h"

namespace Grapple
{
	class Grapple_API Vignette : public RenderPass
	{
	public:
		Vignette();

		void OnRender(RenderingContext& context) override;
	public:
		bool Enabled;
		glm::vec4 Color;
		float Radius;
		float Smoothness;
	private:
		Ref<Shader> m_Shader;
	};
}