#pragma once

//. Basics
#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <string>
#include <cstring>
#include <cstdlib>

//. Data Structures
#include <vector>
#include <variant>
#include <unordered_map>
#include <map>

//. Shell
#ifdef RHY_PLATFORM_WINDOWS
	#include <Windows.h>

#elif defined RHY_PLATFORM_LINUX
	#include <sys/ioctl.h>
	#include <termios.h>
	#include <unistd.h>
#endif

//. Vendor
#include <fmt/core.h>
#include <fmt/format.h>

#include "Core/Core.h"