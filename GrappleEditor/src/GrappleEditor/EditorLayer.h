#pragma once

#include "Grapple.h"
#include "Grapple/Core/Layer.h"

#include "GrappleEditor/UI/SceneWindow.h"
#include "GrappleEditor/UI/PropertiesWindow.h"
#include "GrappleEditor/UI/AssetManagerWindow.h"

namespace Grapple
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
	public:
		virtual void OnAttach() override;
		virtual void OnUpdate(float deltaTime) override;

		virtual void OnEvent(Event& event) override;
		virtual void OnImGUIRender() override;
	private:
		Ref<FrameBuffer> m_FrameBuffer;

		float m_PreviousFrameTime = 0.0f;

		glm::i32vec2 m_ViewportSize = glm::i32vec2(0.0f);

		SceneWindow m_SceneWindow;
		PropertiesWindow m_PropertiesWindow;
		AssetManagerWindow m_AssetManagerWindow;
	};
}