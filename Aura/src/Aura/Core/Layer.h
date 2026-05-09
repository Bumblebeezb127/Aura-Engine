#pragma once

#include "Aura/Core/Core.h"
#include "Aura/Events/Event.h"
#include "Aura/Core/Timestep.h"

namespace Aura
{
	class  Layer {
	public:

		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep timestep) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}
		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}

