#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H


#include"Core.h"

#include "Window.h"
#include "Aura/LayerSatck.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

namespace Aura {

	class AURA_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerSatck m_LayerStack;
	};

	//在客户端定义
	Application* CreateApplication();
}

#endif // !APPLICATION_H
