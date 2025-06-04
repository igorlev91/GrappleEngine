#pragma once

#include "GrappleCore/Core.h"
#include "GrappleCore/Assert.h"

#include <stdint.h>
#include <chrono>

#ifdef Grapple_RELEASE
    #define Grapple_PROFILING_ENABLED
#endif

#include <Tracy.hpp>

#ifdef Grapple_PROFILING_ENABLED
	#define Grapple_PROFILE_BEGIN_FRAME(name) FrameMarkStart(name)
	#define Grapple_PROFILE_END_FRAME(name) FrameMarkEnd(name)

	#define Grapple_PROFILE_SCOPE(name) ZoneScopedN(name)
	#define Grapple_PROFILE_FUNCTION() Grapple_PROFILE_SCOPE(__FUNCSIG__)
#else
    #define Grapple_PROFILE_BEGIN_FRAME(name)
    #define Grapple_PROFILE_END_FRAME(name)
    #define Grapple_PROFILE_SCOPE(name)
    #define Grapple_PROFILE_FUNCTION()
#endif