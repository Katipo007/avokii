#pragma once

#include "PlatformDetection.hpp"

#ifdef _DEBUG // TODO: replace with a custom define
#	if defined( AVOKII_PLATFORM_WINDOWS )
#		define AV_DEBUGBREAK() __debugbreak()
#	else
#		error "Platform does not support debugbreak yet!"
#	endif
#	define AV_ENABLE_ASSERTS 1
#else
#	define DEBUGBREAK() (void)0
#	define AV_ENABLE_ASSERTS 0
#endif

#define AV_EXPAND_MACRO(x) x
#define AV_STRINGIFY_MACRO(x) #x

// these are at the bottom because these files reference this one
#include "Logging.hpp"
#include "Assert.hpp"
