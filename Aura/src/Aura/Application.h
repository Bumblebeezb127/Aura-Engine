#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H


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

#endif // !APPLICATION_H
