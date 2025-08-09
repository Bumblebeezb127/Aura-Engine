#pragma once

#ifdef AR_PLATFORM_WINDOWS

extern Aura::Application* Aura::CreateApplication();

int main(int argc, char** argv) {

	printf("Aura Engine");
	auto app = Aura::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#endif // AR_PLATFORM_WINDOWS
