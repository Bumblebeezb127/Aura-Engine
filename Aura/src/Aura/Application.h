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

	//在客户端定义
	Application* CreateApplication();
}

#endif // !APPLICATION_H
