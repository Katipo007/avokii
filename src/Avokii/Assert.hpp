#pragma once

#include <source_location>

#include "Avokii.hpp"
#include "Logging.hpp"

#if (AV_ENABLE_ASSERTS == 1)

#	define AV_INTERNAL_ASSERT_IMPL(channel, check, msg, ...) do { if(!(check)) { AV_LOG_ERROR( channel, msg, __VA_ARGS__ ); AV_DEBUGBREAK(); } } while(0)
#	define AV_INTERNAL_ASSERT_WITH_MSG(channel, check, ...) { constexpr auto assert_location_ = std::source_location::current(); AV_INTERNAL_ASSERT_IMPL(channel, check, "Assertion failed: {3} in function {0} at {1}:{2}", assert_location_.function_name(), assert_location_.file_name(), assert_location_.line(), __VA_ARGS__); }
#	define AV_INTERNAL_ASSERT_NO_MSG(channel, check) { constexpr auto assert_location_ = std::source_location::current(); AV_INTERNAL_ASSERT_IMPL(channel, check, "Assertion '{0}' failed in function {1} at {2}:{3}", AV_STRINGIFY_MACRO(check), assert_location_.function_name(), assert_location_.file_name(), assert_location_.line()); }

#	define AV_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#	define AV_INTERNAL_ASSERT_GET_MACRO(...) AV_EXPAND_MACRO( AV_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, AV_INTERNAL_ASSERT_WITH_MSG, AV_INTERNAL_ASSERT_NO_MSG) )

// Currently accepts at least the condition and one additional parameter (the message) being optional
#	define AV_ASSERT(...) AV_EXPAND_MACRO( AV_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(::Avokii::LoggingChannels::Assertion, __VA_ARGS__) )
#	define AV_ASSERT_CHANNEL(channel, ...) AV_EXPAND_MACRO( AV_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(channel, __VA_ARGS__) )

#	define AV_NOT_IMPLEMENTED AV_ASSERT( !"Not implemented" )
#else
#	define AV_ASSERT(x, ...) do {} while(0)
#	define AV_ASSERT_CHANNEL(channel, x, ...) do {} while(0)

#	define AV_NOT_IMPLEMENTED do { AV_LOG_ERROR( ::Avokii::LoggingChannels::Assertion, "Not implemented" ); } while(0)
#endif

#define AV_FATAL(msg) do { AV_LOG_CRITICAL( ::Avokii::LoggingChannels::Assertion, "Application has encountered a fatal error and will abort. {0}", msg ); AV_DEBUGBREAK(); abort(); } while(0)
