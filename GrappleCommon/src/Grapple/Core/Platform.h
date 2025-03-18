#pragma once

#ifdef _WIN32
	#ifdef _WIN64
		#define Grapple_PLATFORM_WINDOWS
	#else
		#error "x86 platform is not supported"
	#endif
#endif