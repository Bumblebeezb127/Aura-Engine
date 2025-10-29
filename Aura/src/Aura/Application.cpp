#include "Application.h"
#include "Events/ApplicationEvent.h"
#include "Log.h"

namespace Aura {

	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		WindowResizeEvent event(1280, 720);
		if (event.IsInCategory(EventCategoryApplication)) {

		}
		while (true);
	}
}
