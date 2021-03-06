#pragma once

#include "Avokii/PlatformDetection.hpp"

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

#ifdef AVOKII_PLATFORM_WINDOWS
#	define AVOKII_RENDERER_IMPLEMENTATION_OPENGL
#endif

#define AV_EXPAND_MACRO(x) x
#define AV_STRINGIFY_MACRO(x) #x

// these are at the bottom because these files reference this one
#include "Avokii/Logging.hpp"
#include "Avokii/Assert.hpp"
