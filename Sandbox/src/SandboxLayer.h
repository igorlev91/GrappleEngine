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
		Ref<FrameBuffer> m_FrameBuffer;

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_InverseProjection;

		float m_CameraSize = 24.0f;
		float m_PreviousFrameTime = 0.0f;

		int32_t m_Width = 20;
		int32_t m_Height = 20;
	};
}