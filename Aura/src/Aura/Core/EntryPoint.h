#pragma once

#ifdef AR_PLATFORM_WINDOWS

extern Aura::Application* Aura::CreateApplication();

int main(int argc, char** argv) {
	Aura::Log::Init();
	

	AR_PROFILE_BEGIN_SESSION("Startup", "AuraProfile-Startup.json");
	auto app = Aura::CreateApplication();
	AR_PROFILE_END_SESSION();

	AR_PROFILE_BEGIN_SESSION("Runtime", "AuraProfile-Runtime.json");
	app->Run();
	AR_PROFILE_END_SESSION();

	AR_PROFILE_BEGIN_SESSION("Shutdown", "AuraProfile-Shutdown.json");
	delete app;
	AR_PROFILE_END_SESSION();

	return 0;
}

#endif // AR_PLATFORM_WINDOWS
