#include<Aura.h>


class Sandbox :public Aura::Application {

};

Aura::Application* Aura::CreateApplication() {
	return new Sandbox();
}