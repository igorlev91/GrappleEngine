#pragma once

#ifdef Grapple_BUILD_SHARED
	#define Grapple_API extern "C" __declspec(dllexport)
	#define Grapple_API_CLASS __declspec(dllexport)
#else
	#define Grapple_API
	#define Grapple_API_CLASS
#endif
