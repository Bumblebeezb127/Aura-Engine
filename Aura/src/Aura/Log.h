#pragma once
#ifndef LOG_H
#define LOG_H

#include<memory>
#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Aura {
	class AURA_API Log
	{
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	};
}
// Core log macros
#define AR_CORE_ERROR(...) ::Aura::Log::GetCoreLogger()->error(__VA_ARGS__)
#define AR_CORE_WARN(...)  ::Aura::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define AR_CORE_INFO(...)  ::Aura::Log::GetCoreLogger()->info(__VA_ARGS__)
#define AR_CORE_TRACE(...) ::Aura::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define AR_CORE_FATAL(...) ::Aura::Log::GetCoreLogger()->fatal(__VA_ARGS__)
// Client log macros
#define AR_ERROR(...)      ::Aura::Log::GetClientLogger()->error(__VA_ARGS__)
#define AR_WARN(...)       ::Aura::Log::GetClientLogger()->warn(__VA_ARGS__)
#define AR_INFO(...)       ::Aura::Log::GetClientLogger()->info(__VA_ARGS__)
#define AR_TRACE(...)      ::Aura::Log::GetClientLogger()->trace(__VA_ARGS__)
#define AR_FATAL(...)      ::Aura::Log::GetClientLogger()->fatal(__VA_ARGS__)

#endif // !LOG_H
