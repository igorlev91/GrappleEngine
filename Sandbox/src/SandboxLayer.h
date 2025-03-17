#pragma once

#include "Grapple.h"
#include "Grapple/Core/Layer.h"

namespace Grapple
{
	class SandboxLayer : public Layer
	{
	public:
		SandboxLayer();
	public:
		virtual void OnAttach() override;
		virtual void OnUpdate(float deltaTime) override;

		virtual void OnEvent(Event& event) override;
		virtual void OnImGUIRender() override;
	private:
		void CalculateProjection(float size);
	private:
		Ref<Shader> m_QuadShader;

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_InverseProjection;

		float m_CameraSize = 24.0f;
	};
}