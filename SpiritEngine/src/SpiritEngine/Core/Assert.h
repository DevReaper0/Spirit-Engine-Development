#pragma once

#include "SpiritEngine/Core/Base.h"
#include "SpiritEngine/Core/Log.h"
#include <filesystem>

#ifdef SPIRIT_ENABLE_ASSERTS

	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define SPIRIT_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { SPIRIT##type##ERROR(msg, __VA_ARGS__); SPIRIT_DEBUGBREAK(); } }
	#define SPIRIT_INTERNAL_ASSERT_WITH_MSG(type, check, ...) SPIRIT_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define SPIRIT_INTERNAL_ASSERT_NO_MSG(type, check) SPIRIT_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", SPIRIT_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define SPIRIT_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define SPIRIT_INTERNAL_ASSERT_GET_MACRO(...) SPIRIT_EXPAND_MACRO( SPIRIT_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, SPIRIT_INTERNAL_ASSERT_WITH_MSG, SPIRIT_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define SPIRIT_ASSERT(...) SPIRIT_EXPAND_MACRO( SPIRIT_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define SPIRIT_CORE_ASSERT(...) SPIRIT_EXPAND_MACRO( SPIRIT_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define SPIRIT_ASSERT(...)
	#define SPIRIT_CORE_ASSERT(...)
#endif
