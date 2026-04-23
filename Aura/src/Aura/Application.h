#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H


#include"Core.h"

#include "Window.h"
#include "Aura/LayerSatck.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "Aura/ImGui/ImGuiLayer.h"

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

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerSatck m_LayerStack;
	private:
		static Application* s_Instance;
	};

	//在客户端定义
	Application* CreateApplication();
}

#endif // !APPLICATION_H
