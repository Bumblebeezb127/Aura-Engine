#pragma once

#include "../Core.h"
#include "Event.h"
//#include "../Delegates/Delegate.h"
#include <sstream>

namespace Aura {
	class AURA_API ApplicationEvent : public Event {
	public:
		enum class Type {
			WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
			AppTick, AppUpdate, AppRender
		};
		ApplicationEvent(Type type) : m_Type(type) {}
		Type GetApplicationEventType() const { return m_Type; }
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		Type m_Type;
	};

	class AURA_API WindowCloseEvent : public Event {
	public:
		WindowCloseEvent() {};
		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AURA_API WindowResizeEvent : public Event {
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {
		}
		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }
		std::string ToString() const override {
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}
		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		unsigned int m_Width, m_Height;
	};

	class AURA_API WindowFocusEvent : public ApplicationEvent {
	public:
		WindowFocusEvent() : ApplicationEvent(Type::WindowFocus) {}
		EVENT_CLASS_TYPE(WindowFocus)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AURA_API WindowLostFocusEvent : public ApplicationEvent {
	public:
		WindowLostFocusEvent() : ApplicationEvent(Type::WindowLostFocus) {}
		EVENT_CLASS_TYPE(WindowLostFocus)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AURA_API WindowMovedEvent : public ApplicationEvent {
	public:
		WindowMovedEvent(int x, int y)
			: ApplicationEvent(Type::WindowMoved), m_X(x), m_Y(y) {
		}
		int GetX() const { return m_X; }
		int GetY() const { return m_Y; }
		std::string ToString() const override {
			std::stringstream ss;
			ss << "WindowMovedEvent: " << m_X << ", " << m_Y;
			return ss.str();
		}
		EVENT_CLASS_TYPE(WindowMoved)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		int m_X, m_Y;
	};

	class AURA_API AppTickEvent : public ApplicationEvent {
	public:
		AppTickEvent() : ApplicationEvent(Type::AppTick) {}
		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AURA_API AppUpdateEvent : public ApplicationEvent {
	public:
		AppUpdateEvent() : ApplicationEvent(Type::AppUpdate) {}
		EVENT_CLASS_TYPE(AppUpdate)
	};

	class AURA_API AppRenderEvent : public ApplicationEvent {
	public:
		AppRenderEvent() : ApplicationEvent(Type::AppRender) {}
		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}