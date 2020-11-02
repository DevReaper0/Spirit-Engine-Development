#pragma once

#include "SpiritEngine/Core/Base.h"
#include <glm/glm.hpp>

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)


namespace SpiritEngine {

	class Log
	{
	public:
		static void Init();

		static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};

}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternio)
{
	os << glm::to_string(quaternio);
	return os;
}

// Core log macros
#define SPIRIT_CORE_TRACE(...)    ::SpiritEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SPIRIT_CORE_INFO(...)     ::SpiritEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SPIRIT_CORE_WARN(...)     ::SpiritEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SPIRIT_CORE_ERROR(...)    ::SpiritEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SPIRIT_CORE_CRITICAL(...) ::SpiritEngine::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define SPIRIT_TRACE(...)         ::SpiritEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SPIRIT_INFO(...)          ::SpiritEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define SPIRIT_WARN(...)          ::SpiritEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SPIRIT_ERROR(...)         ::SpiritEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define SPIRIT_CRITICAL(...)      ::SpiritEngine::Log::GetClientLogger()->critical(__VA_ARGS__)
