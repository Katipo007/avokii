#pragma once

#include <SDL2/SDL.h>

#ifdef _WIN64
#	pragma comment(lib, "SDL2/lib/x64/SDL2.lib")
#else
#	pragma comment(lib, "SDL2/lib/x86/SDL2.lib")
#endif
