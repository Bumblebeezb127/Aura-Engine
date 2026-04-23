#pragma once

#include "Aura/Layer.h"

#include "Aura/Events/MouseEvent.h"
#include "Aura/Events/ApplicationEvent.h"
#include "Aura/Events/KeyEvent.h"


namespace Aura
{
	class AURA_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;
		//void OnUpdate();
		//void OnEvent(Event& event);

		void Begin();
		void End();

	private:

		float m_LastFrameTime = 0.0f;
	};
}

