#pragma once

#include "GrapplePlatform/Event.h"

#include <string>

namespace Grapple
{
	class Grapple_API Layer
	{
	public:
		Layer(std::string_view debugName)
			: m_DebugName(debugName) {}

		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnImGUIRender() {}

		virtual void OnEvent(Event& event) {}
	public:
		const std::string& GetDebugName() const { return m_DebugName; }
	private:
		std::string m_DebugName;
	};
}