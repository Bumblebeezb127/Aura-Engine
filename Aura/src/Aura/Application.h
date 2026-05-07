#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H


#include"Core.h"

#include "Window.h"
#include "Aura/LayerSatck.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Aura/Renderer/Shader.h"
#include "Aura/Renderer/Buffer.h"
#include "Aura/Renderer/VertexArray.h"
#include "Aura/Renderer/OrthographicCamera.h"
#include "Aura/ImGui/ImGuiLayer.h"

#include "Aura/Core/Timestep.h"

namespace Aura {

	class  Application
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
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerSatck m_LayerStack;

		Timestep m_Timestep;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};


	Application* CreateApplication();
}

#endif // !APPLICATION_H
