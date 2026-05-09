#pragma once

#include <memory>

#ifdef AR_PLATFORM_WINDOWS
#if AR_DYNAMIC_LINK
	#ifdef AR_BUILD_DLL
		#define  __declspec(dllexport)
	#else 
		#define  __declspec(dllimport)
	#endif 
#else
	#define AURA_API
#endif

#else
	#error Aura only support Windows!
#endif // 

#ifdef AR_DEBUG
	#define AR_ENABLE_ASSERTS
#endif

#ifdef AR_ENABLE_ASSERTS
	#define AR_ASSERT(x, ...) { if(!(x)) { AR_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define AR_CORE_ASSERT(x, ...) { if(!(x)) { AR_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define AR_ASSERT(x, ...)
	#define AR_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define AR_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)


namespace Aura
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}