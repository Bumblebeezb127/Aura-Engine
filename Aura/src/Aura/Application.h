#pragma once

#include"Core.h"

namespace Aura {

	class AURA_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

	};

	//�ڿͻ��˶���
	Application* CreateApplication();
}

