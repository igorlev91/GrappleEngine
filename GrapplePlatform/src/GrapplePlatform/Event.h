#pragma once

#include "GrappleCore/Core.h"

#include <string_view>
#include <functional>

namespace Grapple
{
	enum class EventType
	{
		WindowClose,
		WindowFocus,
		WindowResize,
		WindowMove,

		MouseButtonPressed,
		MouseButtonReleased,

		MouseMove,
		MouseScroll,

		KeyPressed,
		KeyReleased,
		KeyTyped,
	};

	class GrapplePLATFORM_API Event
	{
	public:
		virtual EventType GetType() const = 0;
		virtual std::string_view GetName() const = 0;
	public:
		bool Handled = false;
	};

#define EVENT_CLASS(name) \
	public: \
	virtual EventType GetType() const override { return EventType::##name; } \
	virtual std::string_view GetName() const override { return #name; } \
	static const EventType StaticEventType = EventType::##name; \

#define Grapple_BIND_EVENT_CALLBACK(func) [this](auto&&... args) -> decltype(auto) { return this->func(std::forward<decltype(args)>(args)...); }

	using EventCallback = std::function<void(Event&)>;

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event)
			: m_Event(event) {}
	public:
		template<typename T, typename F>
		bool Dispatch(const F& function)
		{
			if (m_Event.Handled)
				return false;

			EventType type = m_Event.GetType();
			if (m_Event.GetType() == T::StaticEventType)
			{
				m_Event.Handled |= function((T&)m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};
}