#ifdef _WIN32
// windows
#	ifdef _WIN64
#		define AVOKII_PLATFORM_WINDOWS
#	else
#		error "x86 builds are not supported!"
#	endif
#else
#	error "Unsupported platform!"
#endif
