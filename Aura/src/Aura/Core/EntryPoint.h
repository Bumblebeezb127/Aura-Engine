#pragma once

#ifdef AR_PLATFORM_WINDOWS

extern Aura::Application* Aura::CreateApplication();

int main(int argc, char** argv) {
	Aura::Log::Init();
	AR_CORE_WARN("Initialized Log!");
	AR_INFO("Hello!");

	auto app = Aura::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#endif // AR_PLATFORM_WINDOWS
