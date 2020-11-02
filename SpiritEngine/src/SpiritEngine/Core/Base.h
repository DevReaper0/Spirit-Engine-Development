#pragma once

#include <memory>

#include "SpiritEngine/Core/PlatformDetection.h"

#ifdef SPIRIT_DEBUG
	#if defined(SPIRIT_PLATFORM_WINDOWS)
		#define SPIRIT_DEBUGBREAK() __debugbreak()
	#elif defined(SPIRIT_PLATFORM_LINUX)
		#include <signal.h>
		#define SPIRIT_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define SPIRIT_ENABLE_ASSERTS
#else
	#define SPIRIT_DEBUGBREAK()
#endif

#define SPIRIT_EXPAND_MACRO(x) x
#define SPIRIT_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define SPIRIT_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace SpiritEngine {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}

#include "SpiritEngine/Core/Log.h"
#include "SpiritEngine/Core/Assert.h"
