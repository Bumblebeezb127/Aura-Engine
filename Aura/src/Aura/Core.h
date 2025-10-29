#pragma once
#ifdef AR_PLATFORM_WINDOWS
	#ifdef AR_BUILD_DLL
		#define AURA_API __declspec(dllexport)
	#else 
		#define AURA_API __declspec(dllimport)
	#endif // AR_BUILD_DLL
#else
	#error Aura only support Windows!
#endif // 

#define BIT(x) (1 << x)


