#pragma once

#ifdef AVOKII_PLATFORM_WINDOWS
#	include "Platform/Windows/WindowsHeader.hpp"
#	define GLEW_STATIC

#	pragma warning(push, 0) // This ignores all warnings raised inside External headers
#		include "Vendor/glew/include/GL/glew.h"
#		include "Vendor/glew/include/GL/wglew.h"
#	pragma warning(pop)
#else
#	error OpenGL includes not specified for this platform
#endif

