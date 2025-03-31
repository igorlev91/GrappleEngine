#pragma once

#include "Grapple.h"
#include "Grapple/Core/Layer.h"

#include "GrappleEditor/UI/SceneWindow.h"
#include "GrappleEditor/UI/PropertiesWindow.h"
#include "GrappleEditor/UI/AssetManagerWindow.h"

#include "GrappleEditor/ViewportWindow.h"

#include <vector>

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
		float m_PreviousFrameTime = 0.0f;

		SceneWindow m_SceneWindow;
		PropertiesWindow m_PropertiesWindow;
		AssetManagerWindow m_AssetManagerWindow;

		std::vector<ViewportWindow> m_Viewports;
		glm::vec4 m_ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	};
}