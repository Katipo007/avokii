#pragma once

#include <yaml-cpp/include/yaml-cpp/yaml.h>
#ifdef _DEBUG
#	pragma comment(lib, "yaml-cpp/build/Debug/yaml-cppd.lib")
#else
#	pragma comment(lib, "yaml-cpp/build/Release/yaml-cpp.lib")
#endif
