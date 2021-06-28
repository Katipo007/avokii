#pragma once

#ifdef AVOKII_PLATFORM_WINDOWS
#	include "Avokii/Platform/Windows/WindowsHeader.hpp"
#	define GLEW_STATIC

#	pragma warning(push, 0) // This ignores all warnings raised inside External headers
#		include <glew/include/GL/glew.h>
#		include <glew/include/GL/wglew.h>
#	pragma warning(pop)

#	ifdef GLEW_STATIC
#		ifdef _WIN64
#			pragma comment(lib, "glew/lib/Release/x64/glew32s.lib")
#		else
#			pragma comment(lib, "glew/lib/Release/Win32/glew32s.lib")
#		endif
#	else
#		ifdef _WIN64
#			pragma comment(lib, "glew/lib/Release/x64/glew32.lib")
#		else
#			pragma comment(lib, "glew/lib/Release/Win32/glew32.lib")
#		endif
#	endif

// link opengl
#	pragma comment(lib, "opengl32.lib")
#else
#	error OpenGL includes not specified for this platform
#endif

