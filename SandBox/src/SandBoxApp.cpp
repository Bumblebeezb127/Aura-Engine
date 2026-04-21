#include<Aura.h>

class ExampleLayer :public Aura::Layer {
public:
		ExampleLayer() :Layer("Example") {}

		void OnUpdate() override {
			AR_INFO("ExampleLayer::Update");
		}

		void OnEvent(Aura::Event& event) override {
			AR_TRACE("{0}", event.ToString());
		}
};

class Sandbox :public Aura::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}
};

Aura::Application* Aura::CreateApplication() {
	return new Sandbox();
}